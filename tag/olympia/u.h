#pragma once

/*
 *  defines, prototypes and externs for u.c
 */

struct weights {
	int animals;

	int total_weight;	/* total weight of unit or stack */

	int land_cap;		/* carryable weight on land */
	int land_weight;

	int ride_cap;		/* carryable weight on horseback */
	int ride_weight;

	int fly_cap;
	int fly_weight;
};


#define		MATES		(-1)
#define		MATES_SILENT	(-2)
#define		TAKE_ALL	1
#define		TAKE_SOME	2
#define		TAKE_NI		3	/* noble item: wrapper adds one */

void clear_know_rec(sparse *kr);
void olytime_increment(olytime *p);
void olytime_turn_change(olytime *p);
