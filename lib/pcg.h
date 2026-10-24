/*
 * lib/pcg.h -- PCG32 generator + splitmix64 seed mixing.
 *
 * Copied verbatim (code unchanged) from olympia-g3 @ 7b510b4 genesis/pcg.{c,h},
 * where it is genesis-g3's private RNG.  In TAG it is the planned replacement
 * RNG for regenerating the goldens; nothing links it yet.  The engine still uses
 * its inlined drand48/erand48 (olympia/z.c) and mapgen-tag/island-tag the MD5
 * lib/rnd.c + lib/rng.c.  Known-answer test: tests/rng/pcg_check.c.
 *
 * pcg32_seed_step() derives each step's stream from the user seed alone, so
 * steps can be reordered without changing any step's outcome.
 */
#ifndef LIB_PCG_H
#define LIB_PCG_H

#include <stdint.h>

typedef struct {
	uint64_t state;
	uint64_t inc;
} pcg32_t;

/* 64-bit avalanche mixer (identical to olympia/buy.c splitmix64). */
uint64_t splitmix64(uint64_t x);

/* Initialize a generator from an explicit (state, sequence) pair. */
void pcg32_init(pcg32_t *r, uint64_t initstate, uint64_t initseq);

/*
 * Seed a generator for one generation step.  The (seed, step, island) tuple is
 * folded through splitmix64 into a PCG (state, sequence) pair, so each step's
 * stream is a deterministic function of the user seed alone -- reordering steps
 * leaves every step byte-identical.  Pass island = 0 for whole-step draws.
 */
void pcg32_seed_step(pcg32_t *r, uint64_t seed, uint64_t step, uint64_t island);

/* Raw 32-bit draw (advances the stream). */
uint32_t pcg32_next(pcg32_t *r);

/*
 * Unbiased bounded draw in [lo, hi] inclusive (bitmask + rejection), matching
 * the semantics of the legacy rnd(low, high).
 * Requires hi >= lo.
 */
int pcg32_range(pcg32_t *r, int lo, int hi);

#endif /* LIB_PCG_H */
