# Olympia TAG

**TAG** ("The Age of Gods") is the largest of the four Olympia play-by-mail (PBM)
strategy game engines (~80K lines of C). It extends the base game with artifacts,
religion, quests, hero skills, effects, mining, ranger skills, and ship crafting.

Sibling engine repositories:

- [olympia-g1](https://github.com/playbymail/olympia-g1)
- [olympia-g2](https://github.com/playbymail/olympia-g2)
- [olympia-g3](https://github.com/playbymail/olympia-g3)

This repository is a standalone extraction of the TAG engine from the original
multi-engine Olympia monorepo. It builds on its own with CMake.

The code is legacy K&R-style C originally targeting 32-bit systems. The C11/64-bit
modernization effort is complete: `olympia-tag` builds clean under the strict C11
warning ladder (all `-Werror`, zero warnings) and the golden flow runs clean under
AddressSanitizer/UBSan.

> [!IMPORTANT]
> **This is a modernization project, not a development project — no new features.**
> The goal is to bring the existing TAG engine to clean C11 on 64-bit while
> preserving its exact behavior. **Golden output is the contract:** every change
> must keep the golden tests passing (byte-identical), and any behavior change
> must be deliberate, justified, and re-baselined in the same commit. New game
> features, gameplay tweaks, and scope expansion are out of bounds. See
> [BUILD_HISTORY.md](BUILD_HISTORY.md) for the full modernization record.

## Building

Requires CMake (>= 4.1), Ninja, and a Clang or GCC toolchain.

```bash
# Configure + build the default (debug) preset
cmake --preset debug
cmake --build --preset debug
# Binary: build/debug/olympia-tag
```

Other presets are defined in `CMakePresets.json`:

- `debug` — `-O0 -g` (default)
- `release` — `-O3 -DNDEBUG`
- `asan-ubsan` — AddressSanitizer + UndefinedBehaviorSanitizer (`-O1 -g`)

Without presets:

```bash
mkdir build && cd build && cmake .. && cmake --build .
```

### 32-bit build (Linux, for golden-file generation)

```bash
mkdir build32 && cd build32
cmake -DBUILD_32BIT=ON ..   # requires gcc-multilib
cmake --build .
```

## Running / golden tests

`run/olympia-tag.sh` extracts the test fixtures (`tests/olympia/fixtures/lib.tgz`)
into a scratch `run/olympia/lib/` directory, loads the database in immediate mode,
then runs a full `-r -S` turn. `tests/olympia/golden_check.sh` then gates the
post-turn database against a committed sha256 manifest.

```bash
# Build first (default debug preset), then:
./run/olympia-tag.sh                          # extract fixtures, run a turn
./tests/olympia/golden_check.sh               # YES = byte-identical match
./tests/olympia/golden_check.sh --update      # re-baseline after a deliberate change
```

The scripts auto-detect the repo root and look for the binary at
`build/<preset>/olympia-tag` (override the preset with `OLYMPIA_PRESET=release ...`).
The same flow also runs under the `asan-ubsan` preset
(`OLYMPIA_PRESET=asan-ubsan ./run/olympia-tag.sh`) — `YES` plus zero sanitizer
diagnostics.

## Layout

- `olympia/` — the TAG engine sources and headers
- `lib/` — shared support code (entity lists, tiles, roads, effects, allocation, …)
- `tests/` — golden-test fixtures and the golden gate
- `run/` — run/test driver script and scratch run directory
- `doc/` — the TAG reference manual (`r.texi`) and the modernization plan
- `CLAUDE.md` — working guidance (build, test, conventions)
- `BUILD_HISTORY.md` — full phase-by-phase modernization record
- `AUTHORS.md` — authors and credits

## Authors

See [AUTHORS.md](AUTHORS.md). Olympia was created by Rich Skrenta; this
repository is maintained and modernized by Michael Henderson.

## License

GNU Affero General Public License v3 — see [LICENSE](LICENSE). The original
Olympia sources are public domain.
