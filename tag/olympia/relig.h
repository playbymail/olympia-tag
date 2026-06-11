/*
 *  Thu Sep  5 12:12:38 1996 -- Scott Turner
 *
 *  Externs for relig.c
 *
 */
int is_member(int who, int relig);
int use_piety(int who, int amount);
int has_piety(int who, int amount);
int priest_in_stack(int who, int which);
int do_trap_attack(int who, int num_attacks, int chance);
int is_holy_terrain(int n, int where);
