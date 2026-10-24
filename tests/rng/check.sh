#!/bin/bash
###########################################################################
# Known-answer test for lib/pcg.c (PCG32). See pcg_check.c.
#
# Pass:  prints YES and exits 0.
###########################################################################
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${HERE}/../.." && pwd)"
PRESET="${OLYMPIA_PRESET:-debug}"
BIN="${ROOT}/build/${PRESET}/pcg-check-tag"

[ -x "${BIN}" ] || { echo "error: missing ${BIN} (build the ${PRESET} preset first)"; exit 2; }

exec "${BIN}"
