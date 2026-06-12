#!/usr/bin/env bash
set -euo pipefail
############################################################################
#
# golden_check.sh — byte-for-byte gate on the olympia-tag engine output.
#
# The engine writes its post-turn database to run/olympia/lib (a few hundred
# files) — too large and noisy to commit as a literal golden tree. Instead the
# golden is a **hash manifest**: one `<sha256>  <relpath>` line per run file,
# sorted. That captures every byte of every file in a small, diffable artifact.
# Any content change anywhere shows up as a manifest line that no longer matches.
#
# Workflow:
#   1. ./run/olympia-tag.sh           # extract fixtures, run a full -r -S turn,
#                                     # leaving the post-turn DB in run/olympia/lib
#   2. ./tests/olympia/golden_check.sh        # gate: prints YES on a match
#      ./tests/olympia/golden_check.sh --update # (re)capture the baseline
#
# Adapted from olympia-g3's tests/olympia/golden_check.sh. The turn is run with
# the `test-use-const-report-date` flag (see run/olympia-tag.sh) so the Olympia
# Times masthead embeds a fixed date and the manifest is date-independent.
# As on G3, TAG engine output is expected to be deterministic across clean
# rebuilds of byte-identical source, so there is no flaky-file holdout. If a
# build-to-build non-determinism ever appears, add the offending relpath to
# FLAKY_FILES below and commit a `<name>.reference` copy; the loop already knows
# how to hold a file out of the manifest and diff it by hand.
#
############################################################################
#
# Resolve the repo root from this script's location so the repo is relocatable.
OLYMPIA_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OLYMPIA_ENGINE=tag
OLYMPIA_COMMAND=olympia
############################################################################
#
ROOT="${OLYMPIA_ROOT}"
RUN_LIB="${ROOT}/run/${OLYMPIA_COMMAND}/lib"
GOLDEN="${ROOT}/tests/${OLYMPIA_COMMAND}/golden"
MANIFEST="${GOLDEN}/manifest.sha256"

# Known-flaky files: held out of the manifest, diffed by hand against a committed
# reference. EMPTY for TAG (output is expected deterministic across clean
# rebuilds). To add one later: FLAKY_FILES=("fact/100") and commit
# tests/olympia/golden/fact-100.reference
FLAKY_FILES=()

usage() {
  echo "usage: $0 [--update]"
  echo "  --update   replace the golden manifest from current run output"
  exit 2
}

UPDATE=0
if [[ $# -gt 1 ]]; then usage; fi
if [[ $# -eq 1 ]]; then
  [[ "$1" == "--update" ]] || usage
  UPDATE=1
fi

[[ -d "${RUN_LIB}" ]] || { echo "NO: missing run lib dir: ${RUN_LIB} (run ./run/olympia-${OLYMPIA_ENGINE}.sh first)"; exit 1; }

# Pick a sha256 tool (shasum on macOS, sha256sum on Linux; either works on both).
if command -v sha256sum >/dev/null 2>&1; then
  SHA_CMD="sha256sum"
elif command -v shasum >/dev/null 2>&1; then
  SHA_CMD="shasum -a 256"
else
  echo "NO: no sha256 tool (need sha256sum or shasum)"; exit 1
fi

# Build the find prune expression: always skip .DS_Store, plus any FLAKY_FILES.
# (Tree has only simple relative paths — no spaces/newlines — so newline-safe.)
gen_manifest() {
  local prune=( -name .DS_Store )
  local f
  for f in "${FLAKY_FILES[@]:-}"; do
    [[ -n "${f}" ]] && prune+=( -o -path "./${f}" )
  done
  ( cd "${RUN_LIB}" \
      && find . -type f \( "${prune[@]}" \) -prune \
              -o -type f -print \
      | LC_ALL=C sort \
      | xargs ${SHA_CMD} \
      | LC_ALL=C sort )
}

############################################################################
# Update mode: (re)write the golden manifest + any flaky-file references.
if [[ "${UPDATE}" -eq 1 ]]; then
  mkdir -p "${GOLDEN}"
  rm -f "${GOLDEN}/master" "${MANIFEST}"
  gen_manifest > "${MANIFEST}"
  for f in "${FLAKY_FILES[@]:-}"; do
    [[ -n "${f}" && -f "${RUN_LIB}/${f}" ]] && cp "${RUN_LIB}/${f}" "${GOLDEN}/$(echo "${f}" | tr '/' '-').reference"
  done
  echo "OK: updated golden manifest ($(wc -l < "${MANIFEST}" | tr -d ' ') files)"
  exit 0
fi

############################################################################
# Test mode: compare current run output to golden.
[[ -f "${MANIFEST}" ]] || { echo "NO: missing golden manifest: ${MANIFEST}. Run: $0 --update"; exit 1; }

FAIL=0

# 1) Manifest (everything except any flaky files) must match byte-for-byte.
DIFF_OUT="$(diff "${MANIFEST}" <(gen_manifest) || true)"
if [[ -n "${DIFF_OUT}" ]]; then
  echo "NO: run output diverges from golden manifest:"
  echo "${DIFF_OUT}" | head -50
  FAIL=1
fi

# 2) Each flaky file (if any) must match its committed reference exactly.
for f in "${FLAKY_FILES[@]:-}"; do
  [[ -n "${f}" ]] || continue
  REF="${GOLDEN}/$(echo "${f}" | tr '/' '-').reference"
  [[ -f "${REF}" ]] || continue
  if [[ ! -f "${RUN_LIB}/${f}" ]]; then
    echo "NO: missing run file ${f}"; FAIL=1; continue
  fi
  FDIFF="$(diff "${REF}" "${RUN_LIB}/${f}" || true)"
  if [[ -n "${FDIFF}" ]]; then
    echo "NO: ${f} diverges from reference:"; echo "${FDIFF}" | head -50; FAIL=1
  fi
done

if [[ "${FAIL}" -ne 0 ]]; then
  exit 1
fi

echo "YES"
exit 0
