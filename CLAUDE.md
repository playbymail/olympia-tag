# CLAUDE.md

Guidance for working in the **Olympia TAG** repository.

## What this is

TAG (**"The Age of Gods"**) is the **largest** of the four Olympia play-by-mail
(PBM) strategy game engines (~80K lines of C) — it extends the base game with
artifacts, religion, quests, hero skills, effects, mining, ranger skills, and
ship crafting. In the engine lineage it is the **newest**: it descends from G3
(G3's `CLAUDE.md` calls G3 "the ancestor of the TAG engine"). This repo is a
standalone CMake extraction of TAG from the original multi-engine monorepo.

The code is legacy K&R-style C originally targeting **32-bit** systems. The
C11/64-bit modernization effort is **complete**: `olympia-tag` builds clean under
the **entire** strict warning ladder (Phases 1–10), all `-Werror`, zero warnings,
and the golden flow runs clean under AddressSanitizer/UBSan. Read
[Modernization status](#modernization-status) before changing build flags or
touching prototypes — the full phase-by-phase record is in
[BUILD_HISTORY.md](BUILD_HISTORY.md).

> **This is a modernization project, not a development project — no new
> features.** The goal is to preserve TAG's exact behavior while modernizing it
> for 64-bit C11. **Golden output is the contract:** every change must keep the
> golden gate byte-identical, and any behavior change must be deliberate,
> justified, and re-baselined in the same commit. Gameplay tweaks and scope
> expansion are out of bounds.

> **One engine binary, no map generator.** Unlike G3 (three targets:
> `olympia-g3`, `mapgen-g3`, `island-g3`), TAG builds a **single** target,
> `olympia-tag`. There is no `mapgen/` directory and no map/island generator —
> regenerating a map currently means borrowing `olympia-g3`'s generator (the
> `run/olympia-tag.sh` `G3_MAPGEN_SEED_*` vars are a vestige of that). Vendoring
> mapgen into this repo is tracked as **#3** (a separate repo was considered and
> rejected).

## Build

Requires CMake (>= 4.1), Ninja, and Clang or GCC.

```bash
cmake --preset debug
cmake --build --preset debug
# Binary: build/debug/olympia-tag
```

Presets (`CMakePresets.json`): `debug` (default), `release`, `asan-ubsan`.

> **The `debug` build is a plain, non-instrumented build; use the `asan-ubsan`
> preset for sanitizer coverage.** As of Step A (#16), instrumentation flows
> only through `olympia_enable_sanitizers()`, which is gated on `OLYMPIA_SANITIZE`
> — set ON only by the `asan-ubsan` preset. So `debug` builds fast at `-O0 -g`
> with no `-fsanitize`, and you must build/run the `asan-ubsan` preset (which
> sets `OLYMPIA_SANITIZE=ON`, `-O1 -g`) to get ASan/UBSan. Run the sanitized
> engine with `OLYMPIA_PRESET=asan-ubsan ./run/olympia-tag.sh`; expect sanitizer
> output on stderr there, not from the ordinary `debug` binary.

### 32-bit build (Linux only — for regenerating golden files)

Supported via the `BUILD_32BIT` option:

```bash
mkdir build32 && cd build32
cmake -DBUILD_32BIT=ON ..   # requires gcc-multilib; adds -m32
cmake --build .
```

## Test — golden snapshots

`tests/olympia/golden_check.sh` hashes `run/olympia/lib` into a sorted sha256
**manifest** (`tests/olympia/golden/manifest.sha256`, committed). The full
`-r -S` turn in `run/olympia-tag.sh` runs with the `test-use-const-report-date`
flag so the Olympia Times masthead date is fixed and the manifest is
date-independent. Workflow:

```bash
./run/olympia-tag.sh            # extract fixtures (-i), run a full -r -S turn
./tests/olympia/golden_check.sh # prints YES on a byte-identical match
./tests/olympia/golden_check.sh --update   # re-baseline after a deliberate change
```

The contract is the same as the other engines: any change must keep the manifest
byte-identical, and a deliberate output change is re-baselined
(`golden_check.sh --update`) in the same commit with a note on why. The gate also
stands under the `asan-ubsan` preset (`OLYMPIA_PRESET=asan-ubsan
./run/olympia-tag.sh`) — `YES` plus zero sanitizer diagnostics. See
[BUILD_HISTORY.md](BUILD_HISTORY.md#golden-gate) for how the gate was stood up
(the never-run-turn fixes #4/#5/#6, the date flag, and the openrsync blind spot
it sidesteps).

## Layout

- `olympia/` — TAG engine sources (63 `.c`) and headers
- `lib/` — shared support code (entity lists, tiles, roads, allocation, …)
- `tests/` — `olympia/fixtures/lib.tgz` (fixture DB) + the golden gate
  (`olympia/golden_check.sh`, `olympia/golden/manifest.sha256`)
- `run/` — `olympia-tag.sh` driver + scratch run dir (`run/olympia/lib`)
- `doc/` — assorted notes (`r.texi`) + the 64-bit modernization plan
  (`modernization-64bit-plan.md`). The shared prototype **playbook** is *not*
  vendored here — see [Worked examples](#worked-examples--method).
- `CLAUDE.md` — working guidance (build, test, conventions)
- `BUILD_HISTORY.md` — full phase-by-phase modernization record
- `AUTHORS.md` — authors and credits

## Conventions

- Legacy C style: tabs, ANSI prototypes for definitions, terse names. Match the
  surrounding file; don't reformat untouched code.
- **Golden output is the contract.** Modernization changes must produce
  byte-identical output; a deliberate behavior change is re-baselined in the same
  commit with a note.
- Build config lives in `CMakeLists.txt`. As of Step A (#16) enforcement is via
  a single consolidated `olympia_compile_flags()` helper applied to
  `olympia-tag`; sanitizers are wired separately through
  `olympia_enable_sanitizers()` (gated on `OLYMPIA_SANITIZE`, set ON only by the
  `asan-ubsan` preset). The old `legacy_build_flags()` + `phase_1..5_build_flags()`
  blocks and the dead `LEGACY_C_FLAGS*` scaffolding were folded into the one
  helper and deleted.
- **C11 standard** is set both project-wide (`CMAKE_C_STANDARD 11` /
  `…_REQUIRED ON` / `CMAKE_C_EXTENSIONS OFF`, lines 4–6) *and* declared
  explicitly on the `olympia-tag` target via
  `target_compile_features(olympia-tag PRIVATE c_std_11)` (issue #11, merged in
  #13). The per-target call is documentation/intent only — inert because the
  global standard already forces `-std=c11`; it guards against divergence if new
  targets are added. Mirrors the same change in siblings `../olympia-g1` /
  `../olympia-g2` / `../olympia-g3`. Not part of the 64-bit modernization effort.
- **No CI.** Both golden gates are run locally (maintainer decision); warning
  enforcement is not wired into CI.

## Modernization status

The C11/64-bit modernization ran as a phased warning ladder and is **complete**
(epic #15). Every targeted warning class is enforced as `-Werror` via
`olympia_compile_flags()` in `CMakeLists.txt` (applied to the single `olympia-tag`
target), and the golden flow runs clean under AddressSanitizer +
UndefinedBehaviorSanitizer. The per-phase scaffolding has been removed; the
remaining work is the actual 32→64-bit refactoring the ladder cleared the way for.

Enforced classes (all `-Werror`):

| Phase | Scope |
|-------|-------|
| 1 | `int-to-pointer-cast`, `pointer-to-int-cast` |
| 2 | `incompatible-pointer-types` |
| 3 | `int-conversion` |
| 4 | `strict-prototypes`, `missing-prototypes`, `implicit-function-declaration` (per-module headers; no `proto.h`) |
| 5 | `missing-declarations` + sanitizers verified against the golden flow |
| 6 | `shorten-64-to-32` (Clang-guarded) + `sizeof-pointer-memaccess` |
| 7 | `sign-conversion` |
| 8 | `return-type` + `return-mismatch` (Clang-guarded) |
| 9 | `format` / vararg checking, full class incl. format-security (#12) |
| 10 | `implicit-int-conversion` (Clang-guarded, code-quality — not a 64-bit hazard) |

Phases 1–5 were done before the epic; Step A (#16) consolidated the flags and
wired the sanitizer helper, Phases 6–10 (#17/#18/#19/#12/#20) drove the rest of
the ladder, and Step B (#21) is this documentation closeout. A deterministic
newsletter date keeps the goldens reproducible.

**Before changing build flags, prototypes, or headers, read
[BUILD_HISTORY.md](BUILD_HISTORY.md)** — it has the full phase-by-phase record,
the traps each class hid (and the TAG-specific divergences: the single target, the
MD5 RNG inlined in `z.c`, the feature-code format/return sites), the rationale
behind every locked-in flag, and the
[warning policy](BUILD_HISTORY.md#warning-policy) for triaging new warnings.

### Known bugs deferred past the 64-bit effort

The bugs originally recorded here are all **resolved** (full detail, with commit
refs, in [BUILD_HISTORY.md](BUILD_HISTORY.md#known-bugs-deferred-past-the-64-bit-effort)):
#1 (`month_names[-1]` at turn 0, `df5625c`), #2 (`construct_guard_fight_list`
pointer-vs-box-id), #25 (EMOTE format-string injection, `b10ea4d`), #24
(non-literal formats), and #28 (the `assert(FALSE)`-under-`NDEBUG` `release`-build
breakage — fixed with `OLY_UNREACHABLE()` in `legacy.h`, so the `release` preset
builds clean and `-Wuninitialized`/`-Wsometimes-uninitialized` are zero at every
`-O`; the flags are left **informational**, not `-Werror`, as they're
version-sensitive heuristics the warning policy keeps unenforced).

Post-ladder **warning-ratchet** promotions (driven to zero, then locked `-Werror`
outside the phase ladder, per the [warning policy](BUILD_HISTORY.md#warning-policy)):
`-Wchar-subscripts` (#27) and `-Wformat-nonliteral` (#24/#25, incl. the EMOTE
format-string-injection fix).

## Worked examples / method

The shared method, every prototype trap, the golden-gate mechanism, and the
`test-use-const-report-date` date trick live in the sibling playbook (not vendored
here):

```
../olympia-g3/doc/modernization-prototypes-playbook.md
```

TAG's own per-phase write-up — including how it diverged from the siblings — is in
[BUILD_HISTORY.md](BUILD_HISTORY.md); the original ladder plan is in
[doc/modernization-64bit-plan.md](doc/modernization-64bit-plan.md).

**Re-probe; don't assume sibling parity.** TAG differs from G3 in ways that
matter: one target (no mapgen/island), sanitizers gated on the `asan-ubsan`
preset, the `plist` already retired, prototypes via per-module headers (no
`proto.h`), and `effects.c`/`entity_builds.c` **live** here though dead in G3.
Every engine in this lineage has quietly diverged from its sibling — verify TAG's
actual state (build it, grep it, run it under ASan) rather than porting
assumptions.
