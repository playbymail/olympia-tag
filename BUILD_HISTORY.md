# Olympia TAG — Build & Modernization History

Detailed record of the C11/64-bit modernization effort and build-system changes
for the Olympia TAG engine. This is the long-form history; for the current
build/test workflow and conventions see [CLAUDE.md](CLAUDE.md).

TAG ("The Age of Gods") is the largest and newest of the four Olympia
play-by-mail engines (~80K lines of C) — it extends the base game with artifacts,
religion, quests, hero skills, effects, mining, ranger skills, and ship crafting,
and descends from G3. The code originally targeted **32-bit** systems. A phased
warning ladder (Phases 1–10, plus the project-wide format class, flag
consolidation, and wired sanitizers) drove each 32→64-bit hazard class to zero
and locked it as `-Werror`. That ladder is **complete** — every targeted class is
enforced on the single `olympia-tag` target, the golden flow runs clean under
AddressSanitizer + UndefinedBehaviorSanitizer, and the remaining work is the
actual 32→64-bit refactoring the ladder cleared the way for. This file is the
full phase-by-phase record; the [warning policy](#warning-policy) at the end
governs how new warnings are triaged from here.

## Deterministic newsletter date (`test-use-const-report-date`)

`test-use-const-report-date` makes the golden gate deterministic. The Olympia
Times masthead (`times_masthead()` in `olympia/times.c`) otherwise embeds the
wall-clock **date** into `run/olympia/lib/times_0`, so its sha256 diverged from
the committed manifest on any day but the capture day. Passing
`test-use-const-report-date` on the engine command line sets the
`test_use_const_report_date` global, which `main()` pulls out of `argv` **before**
`getopt()` (`olympia/main.c`); `times_masthead()` then emits the fixed date
`January 1, 2000` instead of calling `time()`/`localtime()`. `run/olympia-tag.sh`
passes the flag on the turn run, and the committed manifest was baselined with it,
so `golden_check.sh` is date-independent and prints `YES` on any day. The flag
affects **only** the newsletter date — all other output is byte-identical with or
without it — and normal play (no flag) still prints the real date. (Ported from
G3 when the golden gate was stood up; see [Golden gate](#golden-gate).)

## Modernization status

| Phase | Scope | Issue |
|-------|-------|-------|
| 1 | `int-to-pointer-cast`, `pointer-to-int-cast` | — ✅ (pre-epic) |
| 2 | `incompatible-pointer-types` | — ✅ (pre-epic) |
| 3 | `int-conversion` | — ✅ (pre-epic) |
| 4 | `strict-prototypes`, `missing-prototypes`, `implicit-function-declaration` | — ✅ (pre-epic) |
| 5 | `missing-declarations` + sanitizers | — ✅ (pre-epic) |
| — | Step A: consolidate flags into `olympia_compile_flags()`, fix + wire sanitizers, drop dead scaffolding | #16 ✅ |
| 6 | `shorten-64-to-32` (Clang-guarded) + `sizeof-pointer-memaccess` | #17 ✅ |
| 7 | `sign-conversion` | #18 ✅ |
| 8 | `return-type` + `return-mismatch` | #19 ✅ |
| 9 | format-string / vararg checking (`-Wformat`, full class) | #12 ✅ |
| 10 | `implicit-int-conversion` (Clang-guarded, code-quality) | #20 ✅ |
| — | Step B: write `BUILD_HISTORY.md`, refresh `CLAUDE.md`/`README.md`, fold in warning policy | #21 ✅ |

The whole effort is tracked by epic #15. Every phase was executed one (or two)
commits at a time, with the golden gate `YES` on **both** the `debug` and
`asan-ubsan` presets at every step. The per-phase scaffolding
(`legacy_build_flags()`, `phase_1..5_build_flags()`, `LEGACY_C_FLAGS`,
`LEGACY_C_FLAGS_STRICT`) is gone; the per-phase story lives here and in git
history.

> **TAG-specific lineage facts** (they change which sibling fixes apply):
> - **Single target.** Only `olympia-tag` — no `mapgen`/`island` (unlike G1's
>   two, G3's three). So there is **no multi-target parity phase** (G3's
>   "Phase A"); the ladder starts at Phase 6 on the one target. Regenerating a
>   map currently means borrowing G3's generator; vendoring it is tracked
>   separately (#3) and is not part of this ladder.
> - **MD5 RNG, inlined in `z.c`** (no `rnd.c`, no `drand48`/`erand48`, no
>   `seed[3]`). → **Skip** G1's `seed[3]` signedness fix (there is none, so **no
>   deliberate golden change** in Phase 7), and `sizeof-pointer-memaccess` was
>   already **0** (no MD5 `bzero(ctx, sizeof(ctx))` defect) — Phase 6 just locks
>   it at zero.
> - **Starts further along than G3.** The golden gate already existed and the
>   three never-run-turn bugs (#4 cloudlands hang, #5 `gm_report`
>   div-by-zero/null-deref, #6 Times-index vararg) were already fixed before this
>   epic — **no Step 0**, and Phases 1–5 were already uniformly enforced.
> - **TAG-only feature code** (artifacts, religion, quests, hero/ranger skills,
>   mining, ship crafting) produced fresh format/return/conversion sites with no
>   G1/G2/G3 precedent — every phase was **re-inventoried** by building, not by
>   trusting sibling counts.

> **Caution on the prototype probe:** do **not** use `-Wold-style-definition` to
> find K&R *definitions* — clang reports those under `-Wdeprecated-non-prototype`.
> The full method is in `../olympia-g3/doc/modernization-prototypes-playbook.md`
> (not vendored here). TAG's prototype/declaration work (Phase 4) was already
> done before this epic, via per-module headers — there is **no** generated
> `proto.h` and **0** K&R/empty-paren definitions.

### Phases 1–5 — pointer/int, prototypes, declarations ✅ (pre-epic)

Done before the 64-bit epic (#15) opened, and already locked `-Werror` at **0**
hits on `olympia-tag`:

- **Phase 1** — `int-to-pointer-cast`, `pointer-to-int-cast`.
- **Phase 2** — `incompatible-pointer-types`.
- **Phase 3** — `int-conversion`.
- **Phase 4** — `strict-prototypes`, `missing-prototypes`,
  `implicit-function-declaration`. Prototypes live in **per-module headers**
  (no generated `proto.h`); there are **0** K&R/empty-paren definitions.
- **Phase 5** — `missing-declarations`, plus AddressSanitizer/UBSan available.

Also landed pre-epic and recorded here for context: the golden gate itself
(see [Golden gate](#golden-gate)) and its three prerequisite turn-completion
fixes — the `float_cloudlands()` infinite placement loop (`cloud.c`, #4), the
`gm_report()` div-by-zero/null-deref cluster (`gm.c`, #5), and the
non-deterministic Times-index `%02d` missing-argument write (`times.c`, #6) —
plus the GM-email guard (#9), the typed-empty-string `%s` fix in `gm_land_stats`
(#10), and the retirement of the unused generic `plist` for element-typed lists
(#8, which made the G3-issue-1 mis-cast bug class a compile error). TAG already
shipped with the element-typed lists, so #8 was an audit + dead-file delete, not
G3's field-by-field migration.

### Step A — flag consolidation + fix/wire sanitizers + dead scaffolding removal ✅ (#16)

End-of-runway housekeeping done **first**, so every subsequent phase flag-flip
edits one tidy helper instead of a contradictory multi-block mess. Behavior- and
golden-neutral; its own commit (`1a5827e`).

- **Consolidated the flags.** `legacy_build_flags()` (a big `-Wno-*` block, run
  *before* the phase blocks so contradictions resolved by last-flag-wins) and
  `phase_1..5_build_flags()` were replaced with a single
  `olympia_compile_flags(tgt)` helper applied to `olympia-tag`. The helper carries
  the surviving `-Wno-*` suppressions plus the `-Wfoo -Werror=foo` pairs (one per
  line, `# Phase N` comments). The dead `-Wno-X` entries already overridden by a
  later `-Werror=X` were dropped.
- **Retired the dead scaffolding** (deleted outright): `legacy_build_flags()`,
  `phase_1..5_build_flags()`, `LEGACY_C_FLAGS`, `LEGACY_C_FLAGS_STRICT` — all
  unused once the flags were consolidated.
- **Fixed & wired the sanitizer helper.** ASan/UBSan previously reached the binary
  through a **hardcoded** `-fsanitize=...` block inside `phase_5_build_flags()`, so
  the ordinary `debug` build was instrumented; `olympia_enable_sanitizers()` was
  **defined but never called**, and its `OLYMPIA_SANITIZERS` cache line was
  **malformed** (the exact G2 bug — inert only because the helper was dead). The
  cache line was repaired to the one-line form, the helper is now **called** on
  `olympia-tag`, and the `-fsanitize` block was removed from the phase flags.
  Instrumentation now flows only through `olympia_enable_sanitizers()`, gated on
  `OLYMPIA_SANITIZE` (set ON only by the `asan-ubsan` preset). The `debug` preset
  is now a fast non-instrumented build; `asan-ubsan` is the sanitized one — the
  sibling model, and exactly what the "golden green on **both** presets" invariant
  assumes. `CLAUDE.md`'s "the default debug build already runs under ASan/UBSan"
  claim was corrected in the same step.
- **Optimization owned by the build type.** The old hardcoded `-Og -g` on the
  target was clobbering the preset's `-O`/`-g`. Removed; `CMAKE_BUILD_TYPE` / the
  presets now own it (debug → `-O0 -g`, release → `-O3 -DNDEBUG`, asan-ubsan →
  `-O1 -g`). Golden-neutral — TAG output is deterministic across `-O` levels.

### Phase 6 — `shorten-64-to-32` + `sizeof-pointer-memaccess` ✅ (#17)

`-Wshorten-64-to-32` isolates the core LP64 truncation hazard: a 64-bit value
(`long`/`size_t`/`ssize_t`/`time_t`) silently narrowed into an `int`/`short` —
harmless on the original ILP32 target, lossy on LP64. `-Wsizeof-pointer-memaccess`
catches `sizeof(ptr)` where `sizeof(*ptr)` was meant (8 vs 4 bytes on LP64).

- **`shorten-64-to-32` — 14 sites / 8 files:** `z.c:4, npc.c:3, quest.c:2,
  artifacts.c:1, c2.c:1, check.c:1, eat.c:1, report.c:1`. All
  representation-preserving: `read()` results to `ssize_t`, documented `(int)`
  casts on provably-`<2^31` `strlen` lengths, allocator-size casts. The `z.c`
  MD5/RNG site was eyeballed while here.
- **`sizeof-pointer-memaccess` — 0 sites.** TAG has **no `rnd.c`** (RNG inlined in
  `z.c`) and no `bzero(ctx, sizeof(ctx))` defect; this phase just dropped the
  `-Wno-sizeof-pointer-memaccess` suppression and locked the class at zero.

`shorten-64-to-32` is **Clang-only** (GCC folds it into `-Wconversion`) → guarded
behind `if (CMAKE_C_COMPILER_ID MATCHES "Clang")`. `sizeof-pointer-memaccess` is
portable. Source fixes (`e64b0c6`) and the flag flip (`895b96f`) were separate
commits; golden byte-identical on both presets.

### Phase 7 — `sign-conversion` ✅ (#18)

`-Wsign-conversion` flags implicit signed↔unsigned conversions — architecture-
independent in spelling, but a real hazard where a negative `int` flows into a
`size_t` arg, and a large legacy population.

- **36 sites / 17 files:** `gm.c:7, perm.c:6, report.c:5, combat.c:2, input.c:2,
  main.c:2, use.c:2, check.c:1, day.c:1, garr.c:1, io.c:1, npc.c:1, seed.c:1,
  shipcraft.c:1, sout.c:1, summary.c:1, swear.c:1`. Dominated by `qsort` `nmemb`
  and `my_malloc` size args fed by `int`-returning `*_len()` accessors —
  `(size_t)`/`(unsigned)` casts (values provably ≥ 0), including once in a shared
  `loop.h` macro that cleared several sites at once.
- **No `seed[3]` fix** — TAG is MD5 lineage, not `drand48` → **no deliberate
  golden change** here (unlike G1). Re-checked under `asan-ubsan` after the debug
  pass (G2 found a few `bcopy`/`bzero` length args only the sanitizer surfaces).

Portable (no Clang guard). Source fixes (`8944fe9`) and flag flip (`3768493`)
separate; golden byte-identical on both presets.

### Phase 8 — `return-type` + `return-mismatch` ✅ (#19)

A non-void function that falls off the end, or a bare `return;` in a non-void
function, leaves the caller reading a **garbage register** — worse on LP64 (8
bytes vs 4). This is the class behind G2's `st -32` flicker (`i_use()` fell off
the end and the garbage became `command->status`).

- **`return-type` — 9:** `art.c:2, artifacts.c:1, buy.c:1, combat.c:1, relig.c:1,
  swear.c:1, trading.c:1, use.c:1`. **`return-mismatch` — 2:** `c2.c:1, relig.c:1`.
- Two fix shapes: retype default-`int` procs whose callers ignore the value to
  `void` (definition + every header decl in lockstep); or add the missing
  `return <default>;`. **Trap:** interrupt handlers stored in `cmd_tbl` as
  `int (*)(struct command *)` were **not** retyped to `void` (G2 reverted
  `i_repair`); `use.c`'s `i_*` handlers were checked for the `i_use()`-style
  fall-off. Built `-- -k 0` and re-inventoried until genuinely 0 (Apple clang
  treats `return-mismatch` as error-by-default and `-ferror-limit` truncates the
  first sweep).

`return-type` is portable; `return-mismatch` is **Clang-guarded** (Clang splits
it out of `-Wreturn-type`; GCC enforces the class without the flag, and an
unknown positive `-W*` aborts older GCC — keeping it Clang-only loses no
coverage). Source fixes (`0667db6`) and flag flip (`ceeca05`) separate; golden
byte-identical on both presets.

### Phase 9 — format-string / vararg checking (`-Wformat`) ✅ (#12)

The format / vararg class. A bare `%s`/`%d` with no data argument reads a garbage
stack pointer (and `%s` then derefs it); a width mismatch (`%d` ← `long`/`size_t`)
silently truncates or corrupts on LP64; a non-literal format string lets
`%`-bearing data read the stack. The printf-like wrappers (`out`, `wout`, `wiout`,
`tagout`, `sout`, `log_output`, `html`) carry `__attribute__((format(printf,...)))`
in `legacy.h`, so every call site is checkable — engine-wide `-Wno-format` was the
only thing hiding the class (the same family as the already-fixed `times.c` #6,
`main.c` #9, and `gm.c` #10).

- **46 sites**, deduped by `file:line:col` (matching the independently filed
  inventory in #12). By class: missing-argument (`-Wformat-insufficient-args`,
  memory-unsafe) 9; type mismatch 9; invalid specifier 1; extra/dropped argument
  16 (3 real dropped-output bugs + 13 vestigial leftover args);
  non-literal/`-Wformat-security` 11.
- Remediation grouped by the golden-gate discipline: output-neutral safe fixes
  first (`io.c` `sscanf` intra-struct overflows, `%d`←`long` on stderr, `%zu` for
  `size_t`, the vestigial extra-args), then the memory-unsafe + restored-output
  bugs. Most affected paths (scry/board/blast/specific prayers/storm-strike) don't
  fire in the turn-1 fixture, so the golden delta was small. The `queue` wrapper
  (`legacy.h`) got the missing `format` attribute.
- **Full class locked.** Under Clang `-Wformat` pulls in `-Wformat-security`, so
  dropping `-Wno-format-security` too leaves no sub-suppression (the g2 model) —
  `-Werror=format` now covers the whole class. The non-literal sites that need a
  real dynamic format were handled separately as `-Wformat-nonliteral` (#24/#25),
  now resolved and locked `-Werror` (see [Known bugs](#known-bugs-deferred-past-the-64-bit-effort)).

Source fixes (`37a4b74`, `1588b64`, `30ff71a`) and the flag flip (`cec1a78`) were
separate commits; the few restored player strings were re-baselined in the same
commit with a note, and both presets stayed green.

### Phase 10 — `implicit-int-conversion` (Clang-guarded, code-quality) ✅ (#20)

`-Wimplicit-int-conversion` catches `int` narrowed into a `short`/`schar`/`char`
entity-struct field. This is **not** a 64-bit hazard — behavior is identical on
ILP32 and LP64 — so it sits on a separate **code-quality** track off the critical
path (no `64-bit` label), locked last.

- **171 sites / 29 files**, dominated by **`io.c` (~109 sites)** — the
  entity-restore `switch`/`atoi` tables (`p->field = atoi(t)`) — then a long tail
  across 28 more files. Fixes are explicit `(short)`/`(schar)`/`(char)` casts; a
  compound RHS is wrapped whole, never per-operand.
- **Count trap:** `-Wimplicit-int-conversion-on-negation` is a sub-tag — matched
  by **prefix**, not exact `[-Wimplicit-int-conversion]`, or the inventory is off
  by the negation sites.

`implicit-int-conversion` is **Clang-only** (GCC folds it into `-Wconversion`) →
guarded. Landed as the long tail (`b5fe7e2`) and `io.c` (`0b35447`) as separate
source commits, then the flag flip (`6669352`); representation-preserving, golden
byte-identical on both presets.

## Golden gate

**Golden output is the contract.** Every modernization edit had to keep it
byte-identical; a behavior change that alters output must be deliberate and
re-baselined in the same commit with a note on why.

Two standing gates, both run **locally** (no CI — a maintainer decision):

1. **Debug.** `tests/olympia/golden_check.sh` (adapted from G3) gates the
   post-turn DB in `run/olympia/lib` as a sorted sha256 **manifest**
   (`tests/olympia/golden/manifest.sha256`, committed). Run `./run/olympia-tag.sh`
   (extract fixtures with `-i`, then a full `-r -S` turn), then `golden_check.sh`
   (prints `YES`); re-baseline with `--update`. The manifest is one
   `<sha256>  <relpath>` line per run-output file, so any content change anywhere
   shows up as a line that no longer matches.
2. **asan-ubsan** (standing since the sanitizers were wired in Step A). The same
   flow under ASan/UBSan must run clean: build the `asan-ubsan` preset and re-run
   with `OLYMPIA_PRESET=asan-ubsan ./run/olympia-tag.sh` — the golden check must
   still print `YES` and produce **zero** ASan/UBSan diagnostics. (The
   `ERROR: Can't find mountain 'Mt. Olympus'` lines are benign engine messages,
   not sanitizer output.)

The gate is **date-independent** via the `test-use-const-report-date` flag
(above). TAG output is **deterministic across clean rebuilds** (verified), so the
gate has **no flaky-file holdout** (no G2-style `fact/100` `st -32` flicker); the
harness still knows how to hold a file out of the manifest if build-to-build
non-determinism ever appears.

The gate is also immune to the openrsync `-n -c` dry-run blind spot (issue #7,
origin `playbymail/olympia-g1#4`): an earlier harness style compared run output
to a committed golden tree with `rsync -a -n -c --delete`, but macOS
`/usr/bin/rsync` is openrsync, which in dry-run does **not** honor `-c` and falls
back to a size+mtime quick-check — so an equal-size content change (e.g. the
fixed-width Times masthead date) passed undetected. This gate never used rsync; it
hashes every byte with sha256.

## Known bugs deferred past the 64-bit effort

Tracks intentionally **not** part of the phase ladder, recorded so they aren't
lost:

- **#1 — `month_names[-1]` at turn 0 (`times.c`). RESOLVED (`df5625c`).**
  `times_masthead()` and `player_banner()` (`report.c`) read
  `month_names[oly_month(sysclock)]`, where `oly_month(a) == ((a).turn-1) % 8`, so
  at **turn 0** the index is `-1` → an out-of-bounds read (the array is 8 entries +
  a trailing `NULL` in `glob.c`). Latent under the shipped turn-1 fixture
  (`oly_month()==0`, in bounds), so `asan-ubsan` didn't fire — but it fires on any
  turn-0 DB. Fixed by guarding the index against turn-0/negative before indexing.
  A Phase-5 sanitizer-class finding.
- **#2 — `construct_guard_fight_list` compares `fight *` to an `int` box-id
  (`combat.c`). RESOLVED.** `if (fights_lookup(l_a, (struct fight *)((long)i)) >= 0)`
  compared element pointers to a box-id cast to a pointer, so the lookup always
  returned `-1` and the "don't count a guarding unit already in `l_a`" guard never
  fired. Fixed by reusing TAG's existing `contains_unit(l_a, i)` helper (a
  `->unit` membership scan, the same one `look_for_allies()` uses) instead of the
  broken pointer-compare. Unlike G3 (which lacks `contains_unit` and inlined the
  loop in its Bug #4 fix, `1bfb83f`), `contains_unit` also filters
  `kind == FK_noble` — harmless here since the candidate is always a `T_char` and
  only `FK_noble` entries key `->unit` to a character box-id. **Golden byte-identical**
  on both presets (the turn-1 fixture doesn't exercise the guard path, as on G3),
  so no re-baseline was needed. The corrected behavior is not yet pinned by a
  regression fixture (no fixture triggers combat). Cross-engine parity tracked as
  olympia-g3 issue for refactoring G3 to a shared `contains_unit` helper.
- **#24 / #25 — non-literal format strings (`-Wformat-nonliteral`). RESOLVED;
  class now locked `-Werror`.** Surfaced by the Phase 9 scan but out of scope
  there (`-Wformat-nonliteral` is not part of `-Wformat` on Clang or GCC, so it
  was not covered by the `-Werror=format` lock). 15 sites in three classes, all
  now resolved and the class driven to zero and promoted:
  - **Class C / #25 — security (1).** EMOTE (`v_emote`, `c1.c`) passed raw player
    input (`c->parse[2]`) straight to `wout()` as the format string — a textbook
    format-string injection (CWE-134; `%n` is a stack-write primitive). Fixed in
    `b10ea4d` by emitting the message as data with a literal format, while
    preserving the one documented `%s`→target-name substitution.
  - **Class A — trusted dynamic format (11), `artifacts.c`.** `describe_combat_artifact()`
    / `artifact_identify()` assemble the `wout()` format at runtime from a `header`
    literal + an `artifact_names[]` template (which carries the `%d`/`%s`); trusted
    constants only, never player data, and each switch case supplies the matching
    args. A restructure to inline literals would split the output and break golden,
    so the sites are pragma-annotated in place (`#pragma GCC diagnostic ignored
    "-Wformat-nonliteral"` + a justification comment).
  - **Class B — format passed as a parameter (3), `day.c`/`npc.c`.** Every caller
    passes a literal; the prototypes now carry `__attribute__((format(printf,2,0)))`
    so callers are format-checked, and the forwarding sites are pragma-suppressed
    (clang only auto-suppresses forwarded formats through `va_list`, not when
    re-passed to `wout` with explicit args; the after-params attribute placement
    trips `-Wgcc-compat`, so it's written as a separate prototype).

  With the class at zero, `-Wformat-nonliteral -Werror=format-nonliteral` is now
  locked in `olympia_compile_flags()` (portable; high security value — it stops a
  future EMOTE-style injection from compiling). Golden byte-identical on both
  presets throughout.
- **#28 — `release`/`NDEBUG` build broken by `assert(FALSE)`-as-unreachable.
  RESOLVED.** Legacy code marked unreachable `switch` defaults and function tails
  with `assert(FALSE)`/`assert(0)`. Under `NDEBUG` (any release-type build)
  `assert` is a no-op, so those paths fell through: 14 functions fell off their
  end (`-Werror=return-type`, a hard failure — the `release` preset did not build)
  and 13 switch-assigned variables were read uninitialized
  (`-Wsometimes-uninitialized`). In `debug`/`asan-ubsan` (no `NDEBUG`) `assert(FALSE)`
  is `noreturn`, so neither fired and the golden gate stayed green while `release`
  was red. Fixed with `OLY_UNREACHABLE()` in `legacy.h`
  (`do { assert(0); abort(); } while (0)` — keeps the debug diagnostic, `noreturn`
  under `NDEBUG` too via `abort()`), applied to the 26 terminator-less sites; the
  one non-assert site (`use.c` `being_taught()`'s `teacher`) got `= 0`. **Not** a
  blanket `assert(0)` sweep — the other ~55 sites are in `void` functions or have
  an explicit `assert(0); return x;` fallback and were left alone. `release` now
  builds clean; `-Wuninitialized`/`-Wsometimes-uninitialized` are zero at every
  `-O`. The flags are left **informational** (not `-Werror`) per the warning
  policy — they're version-sensitive heuristics. Golden byte-identical on both
  presets.
- **#29 — `OLY_ASSERT`/`assert(FALSE)`-under-`NDEBUG` audit (follow-up to #28).
  RESOLVED.** The deeper design review and full classification #28 deferred.
  Three outcomes:
  - **Helper design — confirmed as-is, no rename/respelling.** `OLY_UNREACHABLE`
    reads best at the sites (a switch `default:` / function tail is *unreachable*,
    not an asserted *condition* — so it beats `OLY_ASSERT`, and it's a
    can't-happen marker, not a runtime error path, so it beats `oly_panic`). The
    `noreturn`-in-both-configs property comes for free from `abort()` (declared
    `_Noreturn` in `<stdlib.h>`), which already satisfies `-Werror=return-type` on
    **both** Clang and GCC — no explicit attribute is needed (and a bare macro
    can't carry one without becoming a helper function, which buys nothing here).
    `abort()` is kept over `__builtin_unreachable()` deliberately: the contract is
    **fail-loud** (defined abort on a corrupt state), not "let the optimizer assume
    this can't happen" (which would make a wrongly-reached site UB) — the debug
    `assert(0)` still fires first for the file/line diagnostic.
  - **Full 56-site classification (table below).** Every remaining `assert(FALSE)`
    is **(a)** already terminated, **(b)** a `void`/graceful or dead-code site, or
    **(c)** a genuinely-unreachable `switch` default. The audit proved **no
    *mandatory* conversions remain**: the `release` preset builds clean
    (`-Werror=return-type` is CFG-based, not opt-level-sensitive, so any non-`void`
    function that could fall off its end would fail it — none do), and Clang
    reports **zero** `-Wuninitialized`/`-Wsometimes-uninitialized` at `-O0`/`-O1`/
    `-O3` under `NDEBUG`. #28 had already fixed every site where the fall-through
    is *unconditional*; what remained never *definitely* reads uninitialized under
    Clang (each has an intervening conditional escape, is `void`, is dead code —
    e.g. `move.c` `check_captain_loses_sailors` has an unconditional early
    `return;` above its switch — or is under `#if 0`).
  - **4 defensive conversions (bucket (c), opt-in).** Four `switch (rnd(1,N))`
    defaults with every `1..N` case present, where a variable assigned in the
    cases is read after the switch: `art.c:756` (`new_name`), `c1.c:977`/`c1.c:1013`
    (`n`), `quest.c:478` (`s`). These are **mathematically unreachable**, so Clang
    stays silent (the use sits behind a later conditional, so it's not
    *sometimes*-uninitialized) — but GCC's `-Wmaybe-uninitialized` would flag them,
    and they are the exact `assert(FALSE)`-as-unreachable idiom #28 fixed.
    Converted to `OLY_UNREACHABLE()` for defensive consistency: provably
    byte-identical (the default cannot be taken; not exercised by the turn-1
    fixture), pre-empting the latent cross-compiler hazard. The two *defensive*
    enum/kind defaults that read a var after (`art.c:889` `c->use_skill`,
    `input.c:348` `bx[]->kind`) were **left as-is** — their `default:` is
    reachable-in-principle (an unexpected value), so converting would change the
    error-path behavior rather than mark a true impossibility. `swear.c:499`
    (`bribe_thresh = 0`) is already initialized. **No** blanket sweep (the issue's
    ⚠ constraint) — only the four provably-unreachable `rnd(1,N)` sites moved.
    `-Wuninitialized` stays **informational** (not promoted): per the #14 policy
    it's a version/compiler-sensitive heuristic, and GCC would still surface the
    deliberately-unconverted reachable-in-principle defaults. Golden byte-identical
    on `debug` + `asan-ubsan`, sanitizer-clean; `release` builds clean.

  **`assert(FALSE)` site classification** (56 total; the engine has 0 `assert(0)`
  literals — `FALSE` is the engine boolean). After #29 there are 30
  `OLY_UNREACHABLE()` sites (26 from #28 + 4 here) and 52 live `assert(FALSE)`
  sites (56 minus the 4 converted, minus `z.c:1041` commented out and `u.c:1649`
  under `#if 0`, plus those two still grep-match):

  | Bucket | What | Disposition | Sites |
  |--------|------|-------------|-------|
  | (a) terminated | `assert(FALSE)` followed by `return`/`break`/`continue`, or dead after a preceding `return` | leave alone | `basic.c:55,979`, `c1.c:198`, `day.c:864`, `display.c:1179`, `garr.c:419`, `move.c:295,2372`, `relig.c:276`, `report.c:1778`, `quest.c:605,613,623`, `swear.c:583`, `use.c:850,1350` |
  | (b) void / graceful / dead | `void`-function fallthrough, defensive error-check that intentionally continues, or non-compiled code | leave alone | `basic.c:213`, `c1.c:1360,1682`, `code.c:664,738`, `combat.c:4028`, `day.c:122,1495`, `dir.c:759`, `gm.c:693`, `input.c:764`, `io.c:2963`, `kill.c:187,427,580,587`, `lore.c:43`, `move.c:2172` (dead: early `return`), `npc.c:1843`, `perm.c:309,345`, `report.c:1782`, `sout.c:159,342`, `storm.c:127,368,1701`, `swear.c:439,499`, `use.c:503,554`, `quest.c:567` (`#if 0`), `u.c:1649` (`#if 0`), `z.c:1041` (commented) |
  | (c) unreachable `switch` default, var read after — **converted** | `switch (rnd(1,N))` with all cases present | → `OLY_UNREACHABLE()` (this issue) | `art.c:756`, `c1.c:977`, `c1.c:1013`, `quest.c:478` |
  | (c′) defensive enum/kind default, var read after — **left** | `default:` reachable-in-principle (unexpected value) | leave alone (converting would change error-path behavior) | `art.c:889`, `input.c:348` |

## Warning policy

The long-term policy for what is an error, what is suppressed, and how a new
warning is triaged. (This folds in GitHub issue #14, "Establish Post-64-Bit
Warning Policy".) The authoritative list is the conventions block +
`olympia_compile_flags()` in `CMakeLists.txt`; this section is the rationale.

The project deliberately does **not** adopt a blanket global `-Werror`. Compiler
vendors regularly introduce new warnings, and a global `-Werror` turns a routine
compiler upgrade into a broken build even when no functional defect exists.
Instead the project uses a **warning ratchet**: enable a class, drive it to zero,
then promote *that class* to `-Werror` with an explicit `-Wfoo -Werror=foo` pair.
This is exactly the method the Phase 1–10 ladder used.

**Three tiers:**

1. **Enforced (`-Werror`).** Every class on the Phase 1–10 ladder, plus the
   project-wide format class (Phase 9). Each was driven to **zero** on
   `olympia-tag` and locked, written as an explicit `-Wfoo -Werror=foo` pair, one
   class per line — the bare `-Wfoo` is redundant (the `-Werror=` already enables
   it) but is kept deliberately as a record that the class is at zero and locked.
   **Do not** collapse the pairs or relax any to a non-error.

2. **Suppressed (`-Wno-foo`).** Legacy idioms this ~80K-line codebase still leans
   on and that are **not** being chased — each stylistic / low-risk, none a 64-bit
   hazard. The current TAG set in `olympia_compile_flags()`:
   - `-Wno-comment` — `//` inside `/* … */` block comments in legacy source.
   - `-Wno-compare-distinct-pointer-types` — legacy pointer comparisons.
   - `-Wno-dangling-else` — unbraced nested `if/else` in legacy control flow.
   - `-Wno-deprecated-declarations` — deprecated libc calls.
   - `-Wno-deprecated-non-prototype` — clang's spelling for legacy K&R-shaped
     residue (the Phase-4 prototype work is otherwise complete).
   - `-Wno-extra-tokens` — stray tokens after `#endif`/`#else`.
   - `-Wno-incompatible-library-redeclaration` — engine functions whose names
     collide with libc prototypes.
   - `-Wno-logical-not-parentheses` — `!x == y`-style legacy idioms.
   - `-Wno-macro-redefined` — macros redefined across headers.
   - `-Wno-multichar` — multi-character character constants.
   - `-Wno-non-literal-null-conversion` — `0`/`'\0'`-as-pointer idioms.
   - `-Wno-parentheses` — assignment/bitwise precedence without parens.
   - `-Wno-tautological-constant-out-of-range-compare` — narrow-type range
     compares.

   `-Wno-format`/`-Wno-format-security` were **retired** in Phase 9 (the whole
   format class is now enforced). `-Wformat-nonliteral` was the same kind of
   dedicated cleanup track (#24/#25): driven to zero in one focused pass — trusted
   dynamic formats pragma-annotated, forwarded-format wrappers given the `format`
   attribute, the EMOTE injection fixed — then promoted to
   `-Werror=format-nonliteral`.

3. **Clang-only spellings.** `-Wshorten-64-to-32` (Phase 6),
   `-Wreturn-mismatch` (Phase 8), and `-Wimplicit-int-conversion` (Phase 10) are
   Clang-only diagnostics (GCC folds them into broader classes or enforces them by
   default, and treats an unknown positive `-W*` as fatal); they sit behind
   `if (CMAKE_C_COMPILER_ID MATCHES "Clang")` so GCC keeps building with no
   coverage lost.

### Baseline set `-Wall -Wextra -Wpedantic`: evaluated, deliberately not adopted

Issue #14 proposed enabling the broad `-Wall -Wextra -Wpedantic` baseline across
the target. It was **evaluated by building** (Apple clang, keep-going, layered on
the locked ladder) and **deliberately not baked into the build**. Adding the
broad set surfaced **687 diagnostics across 11 classes** — **none** overlapping
the Phase 1–10 ladder and **none** an error (the locked classes are at zero). The
inventory (deduped by `file:line:col`):

| Class | Hits | Kind | Disposition |
|-------|------|------|-------------|
| `-Wextra-semi` | 337 | style (stray `;`) | informational |
| `-Wunused-variable` | 204 | dead code | informational |
| `-Wunused-parameter` | 44 | style (dispatch-table sigs) | informational |
| `-Wunused-but-set-variable` | 32 | dead code | informational |
| `-Wchar-subscripts` | 21 | **portability** (signed `char` index) | fixed + locked `-Werror` (#27) |
| `-Wunused-function` | 20 | dead code | informational |
| `-Wmissing-field-initializers` | 12 | benign (zero-init) | informational |
| `-Wuninitialized` | 8 | **correctness** (UB) | fixed all `-O` (#26 + #28), flag informational by choice |
| `-Wsign-compare` | 5 | low (intent already covered by locked `sign-conversion`) | informational |
| `-Wsometimes-uninitialized` | 3 | **correctness** (2 real defects) | fixed all `-O` (#26 + #28), flag informational by choice |
| `-Wpedantic` | 1 | ISO C nit | informational |

**Decision.** The selected baseline *is* the targeted ratchet set in
`olympia_compile_flags()` — not the broad `-Wall/-Wextra/-Wpedantic`. Baking the
broad set in would add ~687 unenforced informational lines to **every** build,
burying the locked-class signal, while the ratchet already promotes high-value
classes individually. So the broad flags stay **off**; the value of the
evaluation is the inventory above, not a flag change. (Consistent with #14's
non-goals: no warning-free-build requirement, no global `-Werror`.) Because no
build flag changed, this evaluation has **no golden impact** and needed no flag-flip
commit.

**Promotion candidates — outcome** (each its own focused issue):

- **#27 — `-Wchar-subscripts`: fixed and promoted.** All 21 sites were a signed
  `char` passed to the `tolower()` ctype macro (`z.c`/`code.c`); cast each to
  `(unsigned char)` and locked `-Wchar-subscripts -Werror=char-subscripts` in the
  portable block. The first post-64-bit ratchet promotion.
- **#26 — `-Wuninitialized` / `-Wsometimes-uninitialized`: sources fixed, flag NOT
  promoted.** All 11 sites the `-O0` inventory surfaced were fixed: six were
  unambiguous missing initializers (`add.c` log strings → `""`, `build.c` dead
  `id` → 0, `use.c` dispatch `ret` → `FALSE`, `storm.c` `aura_used` accumulator →
  0 and a vestigial `wout(i,…n)` → `target`/`sum`); three required restoring
  disabled/missing game logic and were taken as a **deliberate, approved
  behavior change**, verified golden byte-identical because the turn-1 fixture
  does not exercise them (`c2.c` ferry fee `amount = ship_fee`; `move.c` admission
  `pl = player_controls_loc(...)`, whose only assignment was `#if 0`'d out;
  `day.c` bounty dedup `item_animal_part(choice)`). At the time of #26 the class
  was zero only at `-O0`; `-O1`/`-O3` flow analysis surfaced 13 more "switch
  default falls through" sites — the `assert(FALSE)`-as-unreachable idiom
  degrading under `NDEBUG`, which shared a root cause with the `release`-preset
  `-Werror=return-type` build failure. Both were fixed together under **#28**
  (`OLY_UNREACHABLE()` = `do { assert(0); abort(); } while (0)`, noreturn in every
  build), so the class is now **zero at `-O0`/`-O1`/`-O3`** and the `release`
  preset builds clean. Even so, `-Wuninitialized` is **deliberately left
  informational** (not locked `-Werror`): it's a heuristic, compiler-version-
  sensitive diagnostic, exactly the kind the #14 policy keeps out of the enforced
  set to avoid a routine compiler upgrade breaking the build. The class being at
  zero is a code-quality win, not a reason to lock a fragile flag.

The remaining classes are style / dead-code / benign and are left **informational**
— not tracked individually and not suppressed (they only appear if someone opts
into the broad set locally). `-Wformat-nonliteral` (#24/#25) was a separate cleanup
track and is now **resolved and locked `-Werror`**, recorded above.

**Triaging a new warning** (a compiler upgrade surfaces a new class, or a new
file trips something):

- If it is a **64-bit correctness hazard** (truncation, pointer/int confusion,
  width, sign, missing return → garbage register), treat it like a ladder phase:
  drive the class to **zero**, then add the `-Wfoo -Werror=foo` pair to
  `olympia_compile_flags()`. Never lock a class with live hits remaining.
- If it is **stylistic / low-risk**, either fix the few sites or add a documented
  `-Wno-foo` to the suppression list with a one-line reason. Prefer fixing if the
  count is small.
- **Every** flag change must keep the golden gate `YES` on **both** presets
  (debug and asan-ubsan), byte-identical, asan/ubsan clean — the same invariant
  every ladder phase held. A pure flag flip (enabling/relaxing enforcement) is its
  **own commit**, separate from any structural CMake refactor and from any source
  change.
- **No CI.** Gates are run locally (maintainer decision); warning enforcement is
  not wired into CI.
