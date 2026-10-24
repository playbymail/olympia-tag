/*
 *  lib/prng.h -- the shared rnd() stream, on PCG32 (lib/pcg.c).
 *
 *  One process-global PCG32 stream backs rnd() for all three programs:
 *  olympia-tag (seeded by init_random(), then restored from lib/system by
 *  load_system()) and mapgen-tag / island-tag (seeded from a `randseed` file
 *  by load_seed(), written back by save_seed() so the generators chain).
 *  Replaces the engine's inlined drand48/erand48 and the generators' MD5
 *  lib/rnd.c + lib/rng.c (issue #31).
 *
 *  randseed format: two text lines, `pcg_state=<16 hex>` and
 *  `pcg_inc=<16 hex>` -- the exact stream state, so save/load round-trips.
 *  A file without those keys (e.g. the legacy 16-byte seed phrases in
 *  tests/mapgen/) is hashed as a seed phrase instead.
 */

#ifndef LIB_PRNG_H
#define LIB_PRNG_H

#include <stdint.h>

/*
 *  Draw in [low, high] inclusive.  For high < low, returns what the legacy
 *  drand48 rnd() did -- (int)(u * (high - low + 1) + low), u uniform in
 *  [0, 1) -- from one draw, rather than asserting.
 */
extern int rnd(int low, int high);

/* Seed the stream from a 64-bit value (via pcg32_seed_step). */
extern void rnd_seed(uint64_t seed);

/* Read or restore the exact stream state; rnd_set_state() forces inc odd. */
extern void rnd_get_state(uint64_t *state, uint64_t *inc);
extern void rnd_set_state(uint64_t state, uint64_t inc);

/* randseed file I/O (mapgen-tag / island-tag). */
extern void load_seed(const char *fnam);
extern void save_seed(const char *fnam);

#endif /* LIB_PRNG_H */
