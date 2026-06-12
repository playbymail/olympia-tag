# CLAUDE.md

Guidance for working in the **Olympia TAG** repository.

## What this is

TAG (**"The Age of Gods"**) is the **largest** of the four Olympia play-by-mail
(PBM) strategy game engines (~80K lines of C) — it extends the base game with
artifacts, religion, quests, hero skills, effects, mining, ranger skills, and
ship crafting. In the engine lineage it is the **newest**: it descends from G3
(G3's `CLAUDE.md` calls G3 "the ancestor of the TAG engine"). This repo is a
standalone CMake extraction of TAG from the original multi-engine monorepo.

The code is legacy K&R-style C originally targeting **32-bit** systems, but the
modernization effort here is **much further along than G1/G2/G3**: `olympia-tag`
already builds clean under the **entire** strict C11 flag ladder (Phases 1–5)
**plus AddressSanitizer/UBSan**, all `-Werror`, zero warnings. Read
[Modernization status](#modernization-status) before changing build flags or
touching prototypes — what's left here is *not* what was left on the older
engines.

> **One engine binary, no map generator.** Unlike G3 (three targets:
> `olympia-g3`, `mapgen-g3`, `island-g3`), TAG builds a **single** target,
> `olympia-tag`. There is no `mapgen/` directory and no map/island generator —
> regenerating a map currently means borrowing `olympia-g3`'s generator (the
> `run/olympia-tag.sh` `G3_MAPGEN_SEED_*` vars are a vestige of that). Vendoring
> mapgen into this repo is tracked as **olympia-tag #3** (a separate repo was
> considered and rejected); it pairs with the golden-gate Step 0.

## Build

Requires CMake (>= 4.1), Ninja, and Clang or GCC.

```bash
cmake --preset debug
cmake --build --preset debug
# Binary: build/debug/olympia-tag
```

Presets (`CMakePresets.json`): `debug` (default), `release`, `asan-ubsan`.

> **The default `debug` build already runs under ASan/UBSan.** TAG's
> `phase_5_build_flags()` is applied unconditionally to `olympia-tag` (see
> `CMakeLists.txt` ~line 255), so `-fsanitize=address,undefined` is baked into
> the ordinary debug binary — you do *not* need the `asan-ubsan` preset to get
> sanitizer coverage (it exists for parity with the sibling repos). Expect
> sanitizer output on stderr when you run the engine.

### 32-bit build (Linux only — for regenerating golden files)

Supported via the `BUILD_32BIT` option:

```bash
mkdir build32 && cd build32
cmake -DBUILD_32BIT=ON ..   # requires gcc-multilib; adds -m32
cmake --build .
```

## Test — golden snapshots

> **TAG has no golden gate yet — establishing one is Step 0 (see below).** There
> is only a fixture archive (`tests/olympia/fixtures/lib.tgz`); there is **no**
> `golden_check.sh` and **no** committed manifest, and the full `-r -S` turn is
> even **commented out** in `run/olympia-tag.sh` (only the `-i`
> fixture-extraction phase runs). Before making any change whose contract is
> "byte-identical output," you must first stand up the gate — copy G3's, re-enable
> the turn, and pin the report date. See **Step 0** in Modernization status.

Once the gate exists, the contract is the same as the other engines: any change
must keep it byte-identical, and a deliberate output change is re-baselined in
the same commit with a note on why.

## Layout

- `olympia/` — TAG engine sources (63 `.c`) and headers
- `lib/` — shared support code (entity lists, tiles, roads, allocation, the
  generic `plist`, …)
- `tests/` — `olympia/fixtures/lib.tgz` (fixture DB); **no golden gate yet**
- `run/` — `olympia-tag.sh` driver + scratch run dir (`run/olympia/lib`)
- `doc/` — assorted notes (`r.texi`). The modernization **playbook** is *not*
  vendored here — see [Worked examples](#worked-examples--method).

## Conventions

- Legacy C style: tabs, ANSI prototypes for definitions, terse names. Match the
  surrounding file; don't reformat untouched code.
- **Golden output is the contract** *(once the gate exists)*. Modernization
  changes (dead-code removal, list-type audits) must produce byte-identical
  output; a deliberate behavior change is re-baselined in the same commit with a
  note.
- Build config lives in `CMakeLists.txt`. Enforcement is via the
  `legacy_build_flags()` + `phase_1..5_build_flags()` functions, and on TAG
  **all of them are actually called** on `olympia-tag` (`CMakeLists.txt`
  ~lines 250–255) — unlike G3, where most `phase_*` functions were defined but
  unused scaffolding.

## Modernization status

**TAG is already through Phase 5.** All of the following are `-Werror` on
`olympia-tag` and measure **0** in a clean build (82 TUs, verified):

| Phase | Scope | State |
|-------|-------|-------|
| 1 | `int-to-pointer-cast`, `pointer-to-int-cast` | ✅ enforced |
| 2 | `incompatible-pointer-types` | ✅ enforced |
| 3 | `int-conversion` | ✅ enforced |
| 4 | `strict-prototypes`, `missing-prototypes`, `implicit-function-declaration` | ✅ enforced (prototypes live in per-module headers; there is **no** generated `proto.h`, and there are **0** K&R/empty-paren defs) |
| 5 | `missing-declarations` + ASan/UBSan | ✅ `missing-declarations` enforced; ASan/UBSan baked into the debug build |

So, unlike G1/G2/G3, the prototype/declaration work is **done**. The remaining
runway is short and specific:

1. **Step 0 — establish the golden gate** (prerequisite for everything else).
2. **Phase 3.6 — audit the typed-list migration, then retire the now-unused `plist`.**
3. **Check the four issues filed on the G3 repo** against TAG.

Do them in that order: the gate must exist before the plist deletion or any
issue fix, so byte-identical (or deliberately re-baselined) output can be proven.

### Step 0 — Establish the golden gate (do this FIRST)

1. **Copy the gate.** `../olympia-g3/tests/olympia/golden_check.sh` →
   `tests/olympia/golden_check.sh`; set `OLYMPIA_ENGINE=tag`,
   `OLYMPIA_COMMAND=olympia`. It hashes `run/olympia/lib` into a sorted sha256
   **manifest** (`tests/olympia/golden/manifest.sha256`).
2. **Re-enable the turn.** Un-comment the `-r -S` block in `run/olympia-tag.sh`
   (currently lines ~127–138) so the driver runs a full turn and leaves the
   post-turn DB in `run/olympia/lib`.
3. **Pin the report date** *before* baselining. TAG's "Olympia Times" masthead
   uses the wall-clock date (`times.c` ~lines 128–131: `time()`/`localtime()` →
   `months[tm->tm_mon] …`), so `run/olympia/lib/times_0` embeds *today's* date
   and the manifest would only match on the day it was captured. Port G3's
   **`test-use-const-report-date`** flag (G3 commit `dfd668c`): a global set by a
   long-form arg that `main()` pulls out of `argv` *before* `getopt()`, then
   `times.c` emits a fixed date when it's set. Pass the flag on the turn run in
   `run/olympia-tag.sh`.
4. **Baseline on the pristine tree.** Build, run `-i` then the turn, and
   `golden_check.sh --update`. Re-run across two clean rebuilds to learn TAG's
   inherent determinism (the debug build is sanitized — confirm sanitizer output
   doesn't perturb the `lib/` files, and that two clean runs agree).

See the playbook's **"Verification gate"** section for the date trap and the
same-session-baseline technique.

### Phase 3.6 — Audit the typed-list migration, then retire the unused `plist`

`plist` (`typedef void **plist` in `lib/lists.h` + `lib/plist.c`) is **already
unused dead code** in the TAG engine — `grep -rnE
'plist_(len|clear|reclaim|lookup|append|prepend|insert|copy|delete)|\(plist\)'
olympia/ lib/` (excluding `lib/plist.c` and `lib/lists.h`) comes back **empty**;
the engine already uses the element-typed lists (`fights_list`, `trades_list`,
`exit_views_list`, …). So retiring `plist` is mostly a **dead-file deletion**,
not the field-by-field migration G3 did in its issue #2.

**But do not assume the migration was done correctly — audit it first.** G1, G2,
*and* G3 were each burned by exactly this class of bug: an accessor/element-type
mismatch the compiler cannot catch through `void **` (a typed pointer list read
with `ilist`/`plist` accessors reads the wrong header word and runs off the end).
G3's startup segfault (its issue #1) was precisely a `plist`-queried-as-`ilist`
mismatch hiding in the `loop.h` macros, an `exit_view **` cluster, and an
inventory `qsort`. Before you delete anything:

- **Audit every list site and loop macro for accessor/element-type consistency:**
  - `olympia/loop.h` — the `loop_*` macros declare `ilist ll_l` and iterate with
    `ilist_len`/`ilist_reclaim`. Confirm each one genuinely backs an `int` list,
    not a typed pointer list being read as an `ilist`.
  - `exits_from_loc_nsew*` (`olympia/dir.c`) and *every* consumer — confirm the
    returned list's element type and that each caller iterates it with the
    matching `*_len`/`*_clear`/`*_reclaim`.
  - inventory/sort sites (e.g. `report.c` qsort lengths) — confirm the `*_len`
    matches the list's real element type.
  - Run the turn under the (default) ASan/UBSan build — a wrong-header over-read
    trips the sanitizer.
- **Then delete:** `lib/plist.c`, the `typedef void **plist` and the `plist_*`
  decls in `lib/lists.h`, and the `lib/plist.c` entry in the `olympia-tag`
  `target_sources`. Confirm a clean build, golden green, and ASan-clean. This
  makes the issue-1 bug class a **compile error** — there is no `plist` type left
  to mis-cast through.

> Caution on dead-file removal more broadly: TAG's dead set differs from G3's.
> `lib/ring_buffer.c` is present but **not** in `target_sources` (dead, as in
> G3). But `lib/effects.c` and `lib/entity_builds.c` — which G3 deleted as dead
> in its Phase 3.5 — are **live in TAG** (used by `io.c`/`effect.c`/`display.c`).
> Grep each basename against `CMakeLists.txt` and the tree yourself; don't copy
> G3's deletion list.

### Open issues to check (filed on the G3 repo)

Four issues were filed on `playbymail/olympia-g3`. TAG shares ancestry, so the
agent should check each against TAG (read the full bodies with
`gh issue view <N> --repo playbymail/olympia-g3`):

| G3 issue | Applies to TAG? | Where / what to do |
|----------|-----------------|--------------------|
| **#1** startup segfault (`plist` queried as `ilist` in `post_production`/`location_trades`) | **Likely NO** | TAG's turn already runs to completion under ASan with no crash, and `plist` is unused. Treat as *verify-only*: the Phase 3.6 audit above is the confirmation. |
| **#2** retire the generic `plist` | **YES — but trivial** | This *is* Phase 3.6 above. On TAG it's an audit + dead-file delete, not G3's field-by-field migration (which TAG already effectively has). |
| **#3** `month_names[]` out-of-bounds read in the masthead (ASan global-buffer-overflow) | **YES — confirmed latent** (tracked as **olympia-tag #1**) | `times_masthead()` (`times.c:144`/`149`) reads `month_names[oly_month(sysclock)]`; `oly_month(a) == ((a).turn-1) % 8`, so at **turn 0** it is `-1` → `month_names[-1]` (array is 8 entries + `NULL` at `glob.c:446`). The shipped fixture is turn 1, so `oly_month()==0` and the default ASan `-i` run does *not* fire — but the defect is present. Reproduce on a turn-0 DB; fix by guarding the index against turn 0/negative before indexing. |
| **#4** `construct_guard_fight_list` compares `fight *` pointers to an `int` box-id (guard never fires) | **YES — present** (tracked as **olympia-tag #2**) | `combat.c:1113`: `if (fights_lookup(l_a, (struct fight *)((long)i)) >= 0)` — compares element pointers to a box-id cast to a pointer, so the lookup always returns `-1` and the "don't count a guarding unit already in `l_a`" guard never fires. Fix = a `l_a[j]->unit == i` membership scan (`struct fight` has `int unit`). This **changes combat resolution → changes golden output**, so do it **after** the gate exists and re-baseline with a note. |

### Phase 5 — already enforced

`-Werror=missing-declarations` is on and measures 0, and ASan/UBSan run in the
default debug build. The live work here is reactive: chase any ASan/UBSan finding
that surfaces during the turn, and keep the sanitizer-clean property as you do
Phase 3.6 and the issue fixes. One memory-safety defect is already tracked —
**olympia-tag #1** (`month_names[-1]` at turn 0; latent with the turn-1 fixture,
see the issues table above).

## Worked examples / method

G1, G2, and G3 are each done through Phase 4 (G3 also has the sanitizer/date-flag
work). The shared method, every prototype trap, the **golden-gate Step 0**, and
the **date-stamped-output / `test-use-const-report-date`** trick all live in:

```
../olympia-g3/doc/modernization-prototypes-playbook.md
```

It is **not** vendored into this repo — read it there, or copy it into TAG's
`doc/` if you want it local. G3's `CLAUDE.md` "Modernization status" and its
`Phase 4:` / `test-use-const-report-date` commits are the closest worked example.

**But re-probe; don't assume sibling parity.** TAG already differs from G3 in
ways that matter: one target (no mapgen/island), sanitizers in the *default*
build, `plist` already unused, prototypes via per-module headers (no `proto.h`),
and `effects.c`/`entity_builds.c` **live** here though dead in G3. Every engine
in this lineage has quietly diverged from its sibling — verify TAG's actual state
(build it, grep it, run it under ASan) rather than porting assumptions.
