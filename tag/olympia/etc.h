
int max_eff_aura(int who);				/* art.c */
int has_auraculum(int who);				/* art.c */
void touch_loc_pl(int pl, int where);			/* day.c */
void touch_loc(int who);					/* day.c */
void add_city_trade(int, int, int, int, int, int);	/* buy.c */
int distance(int orig, int dest, int gate);		/* seed.c */
void interrupt_order(int who);				/* input.c */
int may_cookie_npc(int who, int where, int cookie);	/* npc.c */
int do_cookie_npc(int, int, int, int);			/* npc.c */
int weather_here(int where, int sk);			/* storm.c */
void show_char_inventory(int who, int num, char *prefix);/* report.c */
cstrings_list parse_line(cstrings_list l, char *s);			/* input.c */
int unit_maint_cost(int who, int towho);			/* day.c */
char *wield_s(int who);					/* combat.c */
int can_see_weather_here(int who, int where);		/* storm.c */
void queue_lore(int who, int num, int anyway);		/* lore.c */
void create_mine_info(int mine);				/* mining.c */
struct entity_mine *get_mine_info(int where);            /* mining.c */
void copy_file(char *file1, char *file2);                /* main.c */
int nation(int who);                                     /* perm.c */
