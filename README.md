# Olympia TAG

**TAG** ("The Age of Gods") is the largest of the four Olympia play-by-mail (PBM)
strategy game engines (~80K lines of C). It extends the base game with artifacts,
religion, quests, hero skills, effects, mining, ranger skills, and ship crafting.

This repository is a standalone extraction of the TAG engine from the original
multi-engine Olympia monorepo. It builds on its own with CMake.

The code is legacy K&R-style C originally targeting 32-bit systems. A modernization
effort is underway to make it compile cleanly on 64-bit systems; `tag-olympia`
builds clean under the strict C11 flag set plus AddressSanitizer/UBSan.

## Building

Requires CMake (>= 4.1), Ninja, and a Clang or GCC toolchain.

```bash
# Configure + build the default (debug) preset
cmake --preset debug
cmake --build --preset debug
# Binary: build/debug/tag-olympia
```

Other presets are defined in `CMakePresets.json`:

- `debug` — `-Og -g` (default)
- `release`
- `asan-ubsan` — AddressSanitizer + UndefinedBehaviorSanitizer

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

`run/tag-olympia.sh` extracts the test fixtures (`tests/tag/olympia/fixtures/lib.tgz`)
into a scratch `run/tag/olympia/lib/` directory and runs the engine in immediate
mode to confirm it loads the database and executes.

```bash
# Build first (default debug preset), then:
./run/tag-olympia.sh
```

The script auto-detects the repo root and looks for the binary at
`build/<preset>/tag-olympia` (override the preset with `OLYMPIA_PRESET=release ...`).

## Layout

- `tag/olympia/` — the TAG engine sources and headers
- `lib/` — shared support code (entity lists, tiles, roads, effects, allocation, …)
- `tests/tag/` — golden-test fixtures
- `run/` — run/test driver script and scratch run directory
- `tag/doc/` — the TAG reference manual (`r.texi`)

## License

GNU Affero General Public License v3 — see [LICENSE](LICENSE). The original
Olympia sources are public domain.
