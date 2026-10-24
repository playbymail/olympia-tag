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

> **Three targets: the engine plus a vendored map generator.** TAG builds
> `olympia-tag`, `mapgen-tag`, and `island-tag`. The generator (`mapgen/`,
> `island/`, and at the time G3's MD5 RNG in `lib/rnd.c` + `lib/rng.c`) was
> vendored from `olympia-g3` @ `7b510b4` (#3; a separate repo was considered
> and rejected).
> It is byte-identical to G3's except two TAG adaptations: `(unsigned char)`
> `tolower()` casts for TAG's stricter `-Wchar-subscripts` lock, and
> `sub_battlefield` written as `field` (TAG's engine spelling — G3's
> `battlefield` loads as subkind 0 and aborts `loc_depth()`). The golden gate
> still runs on the fixture DB, not on a generated map.

> **One RNG: PCG32, shared by all three programs (#31).** `rnd()` lives in
> `lib/prng.c` on one process-global PCG32 stream (`lib/pcg.c`, copied from G3
> `genesis/` with the code unchanged). It replaced the engine's inlined
> **drand48/erand48** (`olympia/z.c`; *not* MD5, despite what older notes said)
> and the generators' MD5 `lib/rnd.c` + `lib/rng.c`, which are deleted.
> - **Engine seed:** `pcg_state=`/`pcg_inc=` (16 hex digits each) in
>   `lib/system`, the exact stream state, so save → reload continues the
>   stream. A DB with only the legacy drand48 `seed0..2` (like the fixture) is
>   converted once: the 48 bits are folded through `pcg32_seed_step(s, 0, 0)`.
>   `init_random()` seeds from time/pid *before* `load_db()`, so a DB with
>   neither gets a clock seed.
> - **Generator seed:** `randseed` uses the same `pcg_state=`/`pcg_inc=` text.
>   A file without those keys, like the 16-byte phrases in `tests/mapgen/`, is
>   hashed as a seed phrase. `save_seed()` always writes the exact state, so
>   mapgen → island chains.
> - **`rnd(low, high)` with `high < low`** (e.g. `rnd(0, ilist_len(l)-1)` on an
>   empty list, `test_random()`'s `rnd(-3, -10)`) keeps the legacy drand48
>   result, `(int)(u*(high-low+1)+low)` from one draw, rather than asserting.
>   `pcg32_range()` requires `hi >= lo`.

## Build

Requires CMake (>= 4.1), Ninja, and Clang or GCC.

```bash
cmake --preset debug
cmake --build --preset debug
# Binaries: build/debug/{olympia-tag,mapgen-tag,island-tag}
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

### Map generator checks

```bash
./run/mapgen/mapgen.sh                                # Map/Regions/Land/Cities/randseed -> gate/loc/road
./tests/mapgen/regress/secret-sea-route/check.sh      # YES: pinned gate/loc/road sha256 for a fixed seed
./tests/island/regress/ocean-glyphs/check.sh          # YES: island-tag ocean-glyph invariants
```

Inputs live in `tests/mapgen/fixtures/`; `run/mapgen/` output is gitignored.
Both checks honor `OLYMPIA_PRESET` (run them under `asan-ubsan` too).
`secret-sea-route/EXPECT.sha256` pins TAG's own PCG32 output for reproducer seed
`S000000000000005` (since #31 it is no longer comparable to G3's MD5 output). Generated output is **not** part of the engine golden gate: the
fixture DB has no map. (A generated world runs its first turn as-is since #30,
which sets up `lib/log/` before `load_db()`'s world seeding writes GM messages.)

## Layout

- `olympia/` — TAG engine sources (63 `.c`) and headers
- `mapgen/` — map generator (`mapgen.c`, `z.c`; vendored from G3, #3)
- `island/` — island generator (`island.c`; vendored from G3, #3)
- `lib/` — shared support code (entity lists, tiles, roads, allocation, …);
  `prng.c` is the shared `rnd()` stream plus `randseed` I/O, on `pcg.c` (PCG32
  from G3 `genesis/`, held to its known answers by `tests/rng/check.sh`)
- `tests/` — `olympia/fixtures/lib.tgz` (fixture DB) + the golden gate
  (`olympia/golden_check.sh`, `olympia/golden/manifest.sha256`); `mapgen/`
  fixtures + regress check; `island/` regress check
- `run/` — `olympia-tag.sh` driver + scratch run dir (`run/olympia/lib`);
  `mapgen/mapgen.sh` driver + scratch dir
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
- **Never create a branch until we're ready to start working PRs.** Work lands
  directly on `main` (as the modernization history shows); don't spin up a feature
  branch for routine fixes/docs. Only branch once we've explicitly decided to open
  a PR.

## Modernization status

The C11/64-bit modernization ran as a phased warning ladder and is **complete**
(epic #15). Every targeted warning class is enforced as `-Werror` via
`olympia_compile_flags()` in `CMakeLists.txt` (applied to `olympia-tag`, and
since #3 also to `mapgen-tag`/`island-tag`), and the golden flow runs clean under AddressSanitizer +
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
drand48 RNG inlined in `z.c` (since #31, PCG32), the feature-code format/return
sites), the rationale
behind every locked-in flag, and the
[warning policy](BUILD_HISTORY.md#warning-policy) for triaging new warnings.

### Known bugs deferred past the 64-bit effort

The bugs originally recorded here are all **resolved** (full detail, with commit
refs, in [BUILD_HISTORY.md](BUILD_HISTORY.md#known-bugs-deferred-past-the-64-bit-effort)):
#1 (`month_names[-1]` at turn 0, `df5625c`), #2 (`construct_guard_fight_list`
pointer-vs-box-id), #25 (EMOTE format-string injection, `b10ea4d`), #24
(non-literal formats), #28 (the `assert(FALSE)`-under-`NDEBUG` `release`-build
breakage — fixed with `OLY_UNREACHABLE()` in `legacy.h`, so the `release` preset
builds clean and `-Wuninitialized`/`-Wsometimes-uninitialized` are zero at every
`-O`; the flags are left **informational**, not `-Werror`, as they're
version-sensitive heuristics the warning policy keeps unenforced), and #29 (the
follow-up audit: `OLY_UNREACHABLE` confirmed as the right helper — keep the name,
`abort()` over `__builtin_unreachable()` for fail-loud, no extra `noreturn`
attribute needed — plus a full 56-site `assert(FALSE)` classification showing no
*mandatory* conversions remain; 4 mathematically-unreachable `switch (rnd(1,N))`
defaults were converted defensively to pre-empt GCC `-Wmaybe-uninitialized`).

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
matter: one PCG32 `rnd()` shared by engine and generators (`lib/prng.c`; G3
still has MD5), sanitizers gated on the `asan-ubsan`
preset, the `plist` already retired, prototypes via per-module headers (no
`proto.h`), and `effects.c`/`entity_builds.c` **live** here though dead in G3.
Every engine in this lineage has quietly diverged from its sibling — verify TAG's
actual state (build it, grep it, run it under ASan) rather than porting
assumptions.
