/*
 * tests/rng/pcg_check.c -- known-answer test for lib/pcg.c (PCG32).
 *
 * 1. pcg32_init(42, 54) raw outputs must match the reference vector printed by
 *    O'Neill's pcg32-demo (pcg-c-basic, pcg32_srandom_r(&rng, 42u, 54u)).
 * 2. pcg32_range() and pcg32_seed_step() have no published vector; their
 *    expected values were pinned from olympia-g3 @ 7b510b4 genesis/pcg.c, so a
 *    drift from G3's genesis stream fails here.
 *
 * Prints YES and exits 0 on pass; prints the first mismatch and exits 1.
 */
#include <stdio.h>
#include <stdint.h>
#include "pcg.h"

#define NELEM(a)	((int)(sizeof(a) / sizeof((a)[0])))

static const uint32_t ref_raw[] = {
	0xa15c02b7, 0x7b47f409, 0xba1d3330, 0x83d2f293, 0xbfa4784b, 0xcbed606e,
};

/* pcg32_init(42, 54), then pcg32_range(1, 100) x 8. */
static const int pin_range[] = {
	56, 10, 49, 20, 76, 46, 91, 17,
};

/* pcg32_seed_step(18481, i, i % 3), then one pcg32_next(), for i = 0..7. */
static const uint32_t pin_step[] = {
	0xab81a453, 0x8ad9acf4, 0x0f3709db, 0xd73f3988,
	0x8a8e6519, 0x0cbb9ccc, 0x23ac25cf, 0xd1e69f8c,
};

int
main(void)
{
	pcg32_t r;
	int i;

	pcg32_init(&r, 42u, 54u);
	for (i = 0; i < NELEM(ref_raw); i++)
	{
		uint32_t got = pcg32_next(&r);

		if (got != ref_raw[i])
		{
			printf("NO  (raw[%d]: got 0x%08x, want 0x%08x)\n", i,
			    (unsigned)got, (unsigned)ref_raw[i]);
			return 1;
		}
	}

	pcg32_init(&r, 42u, 54u);
	for (i = 0; i < NELEM(pin_range); i++)
	{
		int got = pcg32_range(&r, 1, 100);

		if (got != pin_range[i])
		{
			printf("NO  (range[%d]: got %d, want %d)\n", i, got,
			    pin_range[i]);
			return 1;
		}
	}

	for (i = 0; i < NELEM(pin_step); i++)
	{
		uint32_t got;

		pcg32_seed_step(&r, 18481u, (uint64_t)i, (uint64_t)(i % 3));
		got = pcg32_next(&r);
		if (got != pin_step[i])
		{
			printf("NO  (seed_step[%d]: got 0x%08x, want 0x%08x)\n", i,
			    (unsigned)got, (unsigned)pin_step[i]);
			return 1;
		}
	}

	printf("YES\n");
	return 0;
}
