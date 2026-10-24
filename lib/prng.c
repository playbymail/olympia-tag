/*
 *  lib/prng.c -- the shared rnd() stream, on PCG32.
 *  See lib/prng.h for the contract and the randseed format.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "pcg.h"
#include "prng.h"

/*
 *  The stream.  Starts at the PCG reference default so it is never the
 *  all-zero state (inc == 0 would stick at zero) even if nothing seeds it.
 */
static pcg32_t rng = { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL };

int
rnd(int low, int high)
{
	if (high < low)
		return (int)((double)pcg32_next(&rng) / 4294967296.0
			* (high - low + 1) + low);

	return pcg32_range(&rng, low, high);
}

void
rnd_seed(uint64_t seed)
{
	pcg32_seed_step(&rng, seed, 0, 0);
}

void
rnd_get_state(uint64_t *state, uint64_t *inc)
{
	*state = rng.state;
	*inc = rng.inc;
}

void
rnd_set_state(uint64_t state, uint64_t inc)
{
	rng.state = state;
	rng.inc = inc | 1;
}

/*
 *  Fold a seed phrase into 64 bits, one byte at a time.
 */
static uint64_t
hash_phrase(const unsigned char *buf, size_t len)
{
	uint64_t h = 0;
	size_t i;

	for (i = 0; i < len; i++)
		h = splitmix64(h + buf[i]);

	return h;
}

void
load_seed(const char *fnam)
{
	FILE *fd;
	unsigned char buf[1024];
	size_t len;
	char *s;
	uint64_t state = 0;
	uint64_t inc = 0;
	int have_state = 0;
	int have_inc = 0;

	fd = fopen(fnam, "rb");
	if (fd == NULL) {
		printf("%s could not be opened.\n", fnam);
		return;
	}

	len = fread(buf, 1, sizeof(buf) - 1, fd);
	fclose(fd);
	buf[len] = '\0';

	s = (char *)buf;
	while (s) {
		if (strncmp(s, "pcg_state=", 10) == 0) {
			state = strtoull(&s[10], NULL, 16);
			have_state = 1;
		} else if (strncmp(s, "pcg_inc=", 8) == 0) {
			inc = strtoull(&s[8], NULL, 16);
			have_inc = 1;
		}

		s = strchr(s, '\n');
		if (s)
			s++;
	}

	if (have_state && have_inc)
		rnd_set_state(state, inc);
	else
		rnd_seed(hash_phrase(buf, len));
}

void
save_seed(const char *fnam)
{
	FILE *fd;

	fd = fopen(fnam, "w");
	if (fd == NULL) {
		printf("%s could not be written.\n", fnam);
		return;
	}

	fprintf(fd, "pcg_state=%016" PRIx64 "\n", rng.state);
	fprintf(fd, "pcg_inc=%016" PRIx64 "\n", rng.inc);
	fclose(fd);
}
