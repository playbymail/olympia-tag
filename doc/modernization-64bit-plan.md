# Olympia TAG — 64-bit / C11 warning-ladder modernization plan

Bring `olympia-tag` to the same finished state as `../olympia-g1` and
`../olympia-g2` (and that `../olympia-g3` is mid-way through): the full strict
warning ladder driven to **zero** and locked `-Werror`, sanitizers wired, the
flag set consolidated, and a `BUILD_HISTORY.md` recording it.

Everything below was **verified by building**, not by reading CMake comments or
issue labels. Compiler: Apple clang 21.0.0. Counts are deduplicated by
`file:line:col` (the raw per-TU tally double-counts header/macro sites ~2×); the
dedup total for the format family is **46**, which matches the independently
filed epic (#12) exactly — that calibrates the method.

---

## 1. Verified current ladder position

**Phases 1–5 are genuinely done and locked** on the single `olympia-tag` target.
A clean debug build is `-Werror` with **0** hits for:

| Phase | Class(es) | State |
|-------|-----------|-------|
| 1 | `int-to-pointer-cast`, `pointer-to-int-cast` | ✅ `-Werror`, 0 |
| 2 | `incompatible-pointer-types` | ✅ `-Werror`, 0 |
| 3 | `int-conversion` | ✅ `-Werror`, 0 |
| 4 | `strict-prototypes`, `missing-prototypes`, `implicit-function-declaration` | ✅ `-Werror`, 0 (per-module headers; no `proto.h`) |
| 5 | `missing-declarations` + ASan/UBSan | ✅ `-Werror`, 0; sanitizers baked in |

**Golden gate green on BOTH presets, byte-identical, sanitizers clean:**
- `debug`: `./run/olympia-tag.sh` + `./tests/olympia/golden_check.sh` → `YES`.
- `asan-ubsan`: `OLYMPIA_PRESET=asan-ubsan ./run/olympia-tag.sh` + gate → `YES`,
  run exit 0, **zero** ASan/UBSan diagnostics. (The `ERROR: Can't find mountain
  'Mt. Olympus'` lines are benign engine messages, not sanitizer output.)

**How the flag blocks actually resolve.** `CMakeLists.txt` calls
`legacy_build_flags()` (a big `-Wno-*` block) *first* (line 253), then
`phase_1..5_build_flags()` (254–258). CMake/clang honor **last flag wins**, so
every `-Wno-X` in `legacy_build_flags` that has a later `-Werror=X` is overridden
to **enforced** (int-conversion, the pointer casts, implicit-function-declaration,
incompatible-pointer-types). No class is genuinely "suppressed *and* enforced" —
the contradictions resolve cleanly, but they are confusing and the consolidation
step removes them.

**Sanitizer wiring — important divergence from siblings.** ASan/UBSan reach the
binary through the **hardcoded** `phase_5_build_flags()` (unconditional
`-fsanitize=address,undefined`, applied to the ordinary `debug` build), **not**
through `olympia_enable_sanitizers()`. That helper is **defined but never
called**, and its `OLYMPIA_SANITIZERS` cache line is **malformed** (the exact G2
bug: a stray `address,undefined` default after the docstring) — but the bug is
**inert** here because the helper is dead. Both presets instrument identically
(11 963 ASan symbols in each binary); the `asan-ubsan` preset only differs by
`-O1` vs `-Og`.

**Dead scaffolding (confirmed unused):**
- `LEGACY_C_FLAGS` / `LEGACY_C_FLAGS_STRICT` — defined (lines 136–175), never
  applied to any target.
- `olympia_enable_sanitizers()` + `option(OLYMPIA_SANITIZE)` + the malformed
  `OLYMPIA_SANITIZERS` cache var — never called.
- Hardcoded `-Og -g` on the target (line 250) clobbers what the build type/preset
  should own.

---

## 2. Remaining ladder — real, deduplicated hit counts

Phase order follows the proven sibling ladder. Each row is a class **currently
not locked**. "Suppressed" = a live `-Wno-` in `legacy_build_flags`; "silent" =
not in the default `-W` set, so it never fired until probed.

| Phase | Class | Sites | Files | Currently | Clang-guard? | Golden |
|-------|-------|------:|------:|-----------|--------------|--------|
| **6** | `shorten-64-to-32` | **14** | 8 | silent | **yes** (GCC folds into `-Wconversion`) | byte-identical |
| **6** | `sizeof-pointer-memaccess` | **0** | 0 | suppressed (`-Wno-`) | no (portable) | n/a — lock at zero |
| **7** | `sign-conversion` | **36** | 17 | silent | no | byte-identical |
| **8** | `return-type` | **9** | 8 | suppressed (`-Wno-`) | no | byte-identical |
| **8** | `return-mismatch` | **2** | 2 | suppressed (`-Wno-`) | clang naming; keep portable unless GCC objects | byte-identical |
| **9** | format family (`-Wformat` + sub-tags) | **46** | — | suppressed (`-Wno-format`,`-Wno-format-security`) | no | mostly byte-identical; some restored player text → re-baseline |
| **10** | `implicit-int-conversion` | **171** | 29 | silent | **yes** (GCC folds into `-Wconversion`) | byte-identical |

### Per-phase site breakdown (deduped)

- **Phase 6 — `shorten-64-to-32` (14):** `z.c:4, npc.c:3, quest.c:2,
  artifacts.c:1, c2.c:1, check.c:1, eat.c:1, report.c:1`. All
  representation-preserving (`read()`→`ssize_t`, documented `(int)` casts on
  provably-`<2^31` `strlen` lengths, allocator-size casts). `sizeof-pointer-memaccess`
  is already **0** — TAG has **no `rnd.c`**; the RNG lives inline in `z.c`, and no
  `bzero(ctx, sizeof(ctx))` site exists. Verify the `z.c` MD5/RNG site by eye
  while here, then lock the pair at zero by dropping `-Wno-sizeof-pointer-memaccess`.
- **Phase 7 — `sign-conversion` (36):** `gm.c:7, perm.c:6, report.c:5, combat.c:2,
  input.c:2, main.c:2, use.c:2, check.c:1, day.c:1, garr.c:1, io.c:1, npc.c:1,
  seed.c:1, shipcraft.c:1, sout.c:1, summary.c:1, swear.c:1`. Dominated by
  `qsort` `nmemb` and `my_malloc` size args fed by `int`-returning `*_len()`
  accessors — `(size_t)`/`(unsigned)` casts, including once in a shared `loop.h`
  macro that clears several sites at once. **No `seed[3]` fix** (TAG is MD5
  lineage, not `drand48`) → no deliberate golden change here. Re-check under
  `asan-ubsan` (G2 found a few `bcopy`/`bzero` length args only the sanitizer
  surfaces).
- **Phase 8 — `return-type` (9) + `return-mismatch` (2):** `return-type` in
  `art.c:2, artifacts.c:1, buy.c:1, combat.c:1, relig.c:1, swear.c:1, trading.c:1,
  use.c:1`; `return-mismatch` in `c2.c:1, relig.c:1`. Two fix shapes: retype
  default-`int` procs whose callers ignore the value to `void` (def + every header
  decl in lockstep), or add the missing `return <default>;`. **Trap:** do **not**
  retype interrupt handlers stored in `cmd_tbl` as `int (*)(struct command *)` to
  `void` (G2 reverted `i_repair`) — check `use.c`'s `i_*`/interrupt handlers for
  the `i_use()`-style fall-off that caused G2's `st -32` flicker. Build `-- -k 0`
  and re-inventory until truly 0 (Apple clang treats `return-mismatch` as
  error-by-default and `-ferror-limit` truncates the first pass).
- **Phase 9 — format family (46) — existing epic #12:** `-Wformat` 9, extra-args
  16, insufficient-args 9, invalid-specifier 1, format-security 11. The
  **insufficient-args** sites are the memory-unsafe ones (bare `%s` reading a
  garbage pointer): `relig.c:3, scry.c:2, artifacts.c:1, c2.c:1, display.c:1,
  necro.c:1`. Replace `-Wno-format` with `-Wformat -Werror=format`. Most fixes are
  byte-identical (affected text isn't in the 30-file snapshot); any **restored**
  player-facing string is a deliberate re-baseline in the same commit with a note.
  The 11 `format-security` non-literal `out(who, buf)` sites may be deferred behind
  `-Wno-format-security` for a later hardening pass, matching the siblings.
- **Phase 10 — `implicit-int-conversion` (171):** dominated by `io.c` (~109 of
  them — the entity-restore `switch`/`atoi` tables), then a long tail across 28
  more files. Code-quality, **not** a 64-bit hazard (`int`→`short`/`char` is
  identical on ILP32/LP64). Land `io.c` and the long tail as separate commits.
  **Count trap:** `-Wimplicit-int-conversion-on-negation` is a sub-tag — match by
  **prefix**, not exact `[-Wimplicit-int-conversion]`. Clang-guarded, locked last.

---

## 3. Structural cleanup (behavior-neutral) — **goes FIRST** (decision)

Done **before Phase 6** so every subsequent phase flag-flip edits one tidy helper
instead of the contradictory multi-block mess. Pure structural refactor: golden
`YES` unchanged on both presets, its own commit(s).

- **Consolidate** `legacy_build_flags()` + `phase_1..5_build_flags()` into one
  `olympia_compile_flags(tgt)` helper applied to the single target; keep one
  `-Wfoo -Werror=foo` pair per line as the record that the class is at zero and
  locked. Drop the now-dead `-Wno-` entries that a later `-Werror=` already
  overrides.
- **Delete dead scaffolding:** `LEGACY_C_FLAGS`, `LEGACY_C_FLAGS_STRICT`, and the
  unused `phase_*` functions once inlined.
- **Sanitizers — fix & wire the helper (decision):** repair the malformed
  `OLYMPIA_SANITIZERS` cache line to the proper one-line form
  (`set(OLYMPIA_SANITIZERS "address,undefined" CACHE STRING "...")` — the G2 fix),
  **call** `olympia_enable_sanitizers()` on `olympia-tag`, and **remove** the
  `-fsanitize` block from `phase_5_build_flags()` so the instrumentation flows
  through the helper. With `OLYMPIA_SANITIZE` gated by the preset, the `debug`
  preset becomes a fast non-instrumented build and `asan-ubsan` is the sanitized
  one — the sibling model, and exactly what the "golden green on **both** presets"
  invariant assumes. **Doc impact:** this falsifies CLAUDE.md's "the default debug
  build already runs under ASan/UBSan" claim, so update that sentence (and the
  `run`/test guidance to use `OLYMPIA_PRESET=asan-ubsan` for the sanitizer gate) in
  the **same** structural commit. Golden output is sanitizer-independent, so this
  stays byte-identical.
- **Let the build type own `-O`/`-g`:** remove hardcoded `-Og -g`; debug→`-O0 -g`,
  release→`-O3 -DNDEBUG`, asan-ubsan→`-O1 -g` (from the preset). Golden is
  opt-level-independent.

## 3b. Docs — goes LAST

- **`BUILD_HISTORY.md`:** mirror the G1/G2 structure (per-phase: class, why it's a
  32→64 hazard, traps, representation-preserving rationale; the three-tier warning
  policy; the golden-gate mechanism; TAG-specific divergences). Fold in the
  post-64-bit warning-policy text (existing issue #14). Refresh `CLAUDE.md`'s
  modernization table to "complete".

---

## 4. TAG-specific divergences (verified)

1. **Single target.** Only `olympia-tag` — no `mapgen`/`island`. So there is **no
   multi-target parity phase** (G3's "Phase A"); Phases 1–5 are already uniformly
   enforced on the one target. The plan starts at **Phase 6**.
2. **Starts further along than G3.** The golden gate already exists (Step 0 done),
   and the three never-run-turn bugs (#4 cloudlands hang, #5 `gm_report`
   div-by-zero/null-deref, #6 Times-index vararg) are already fixed. No Step 0.
3. **MD5 lineage, RNG inlined in `z.c`** (no `rnd.c`, no `drand48`/`erand48`, no
   `seed[3]`). → **Skip** G1's `seed[3]` signedness fix. `sizeof-pointer-memaccess`
   is already **0** (no MD5 `bzero(ctx,sizeof(ctx))` defect) — Phase 6 just locks
   it at zero.
4. **Sanitizers in the *default* build** via the `phase_5` hardcode; the
   `olympia_enable_sanitizers()` machinery is dead + malformed but inert.
5. **Format epic already filed (#12, 46 sites).** Fold it in as **Phase 9**;
   do not duplicate it.
6. **Existing latent bugs to weave in (not new ladder phases):**
   - **#1** `month_names[-1]` at turn 0 (`times.c`) — a Phase-5/`memory-safety`
     sanitizer finding, latent under the turn-1 fixture. Reference from the epic;
     fixable opportunistically.
   - **#2** `construct_guard_fight_list` compares `fight *` to an `int` box-id
     (`combat.c:1113`) — a real 64-bit/golden defect. Fixing it **changes combat
     resolution → changes golden**, so do it after the gate (which exists) and
     re-baseline with a note. Relate it to the ladder (Phase 7/8 territory).
   - **#3** vendor mapgen — orthogonal infrastructure, not part of this ladder.

---

## 5. Proposed phase / commit order

```
Structural  consolidate flags + fix/wire sanitizers + build-type owns -O/-g   [FIRST]
   ↓
Phase 6  shorten-64-to-32 (+ lock sizeof-pointer-memaccess at 0)   [14 sites]
   ↓
Phase 7  sign-conversion                                           [36 sites]
   ↓
Phase 8  return-type + return-mismatch                             [11 sites]
   ↓
Phase 9  format / vararg  (existing #12)                           [46 sites]
   ↓
Phase 10 implicit-int-conversion (Clang-guarded, code-quality)     [171 sites]
   ↓
BUILD_HISTORY.md + CLAUDE.md refresh (folds #14 policy)
```

Each **phase** is generally **two commits**: (a) source fixes driven to zero
(usually byte-identical golden), then (b) the **pure flag flip** — remove the
`-Wno-X`, add the `-WX -Werror=X` pair — as its own commit. Structural CMake
refactor is its own commit, separate from any source change.

---

## 6. Per-commit invariants (non-negotiable)

1. Golden gate `= YES` (byte-identical) on **both** `debug` and `asan-ubsan` at
   every commit. A deliberate output change (a restored player string; the #2
   combat fix) is re-baselined via `golden_check.sh --update` **in the same
   commit**, with a note explaining why.
2. Sanitizers clean (run exit 0, zero diagnostics) at every commit.
3. Drive a class to **zero** across the whole build *before* locking its
   `-Wfoo -Werror=foo` pair. Never lock a class with live hits. Inventory with
   `-- -k 0` and re-sweep until the count is stable (Apple clang `-ferror-limit`
   truncates).
4. A pure flag flip (`-Wno-` → `-Werror=`) is its **own** commit, separate from
   source fixes and from the structural CMake refactor.
5. Clang-only diagnostics (`shorten-64-to-32`, `implicit-int-conversion`, and
   `return-mismatch` if GCC objects) go behind
   `if (CMAKE_C_COMPILER_ID MATCHES "Clang")`.
6. No CI, no branches, no PRs — commit on `main`. Avoid GitHub auto-close keywords
   in commit/issue text unless closure is intended.
7. **Pause for review** at each golden-gate checkpoint (each phase).

---

## 7. Issues to file (mirroring olympia-g3 #10–#18)

One **epic** + one issue per remaining phase, dependency-ordered, cross-linked
both ways (epic carries a `- [ ] #N — … (depends on #M)` checklist; each child
opens `Part of #<epic>. **Depends on #<prev>.**`). Reuse existing issues; don't
duplicate.

| New issue | Title | Labels | Depends on |
|-----------|-------|--------|-----------|
| Epic | Epic: bring olympia-tag to G1/G2 64-bit modernization parity | `modernization`, `64-bit`, `area: build` | — |
| Structural | Step A: consolidate flags into olympia_compile_flags() + fix/wire sanitizers + drop dead scaffolding | `modernization`, `area: build` | — (first step) |
| Phase 6 | Phase 6: shorten-64-to-32 (Clang-guarded) + sizeof-pointer-memaccess | `modernization`, `64-bit` (+ new `phase: 6`) | Structural |
| Phase 7 | Phase 7: sign-conversion | `modernization`, `64-bit` (+ `phase: 7`) | Phase 6 |
| Phase 8 | Phase 8: return-type + return-mismatch | `modernization`, `64-bit` (+ `phase: 8`) | Phase 7 |
| **#12** (reuse) | Phase 9: format / vararg (existing) | add `modernization` (+ `phase: 9`) | Phase 8 |
| Phase 10 | Phase 10: implicit-int-conversion (Clang-guarded, code-quality) | `modernization`, `area: build` (+ `phase: 10`) | Phase 8 |
| Docs | Step B: write BUILD_HISTORY.md + refresh CLAUDE.md | `modernization`, `documentation` | all; folds #14 |

`64-bit` only on true hazard phases (6/7/8 + epic), **not** on Phase 10
(code-quality), Structural (behavior-neutral), or Docs. New `phase: 6/7/8/10`
labels needed (repo currently has only `phase: 4`, `phase: 5`). Epic also
references #1, #2, #3, #14 as related. Existing #12 is folded in as Phase 9 (add
labels, prepend a `Part of #<epic>` line), and #14 folds into the Docs step.
