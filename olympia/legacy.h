// Copyright (c) 2026 Michael D Henderson. All rights reserved.

//
// Created by Michael Henderson on 1/25/26.
//

#ifndef OLYMPIA_LEGACY_H
#define OLYMPIA_LEGACY_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include "../lib/lists.h"

/* BUGFIX (modernization): forward declarations */
struct command;
struct entity_build;
struct loc_control_ent;
struct skill_ent;
struct weights;

/* add.c */
void add_new_players(void);
void calculate_nation_nps(void);
void new_player_list(void);
void new_player_top(int mail);
int nps_invested(int who);
int pick_starting_city(int nat, int start_city);
void rename_act_join_files(void);
int starting_gold(int nation);
int starting_noble_points(int nation);

/* alchem.c */
int d_brew_death(struct command *c);
int d_brew_fiery(struct command *c);
int d_brew_heal(struct command *c);
int d_brew_slave(struct command *c);
int d_brew_weightlessness(struct command *c);
int d_lead_to_gold(struct command *c);
int new_potion(int who);
int v_brew(struct command *c);
int v_lead_to_gold(struct command *c);
int v_use_death(struct command *c);
int v_use_fiery(struct command *c);
int v_use_heal(struct command *c);
int v_use_slave(struct command *c);
int v_use_weightlessness(struct command *c);

/* adv.c */
int d_create_iron_golem(struct command *c);
int d_teleport_item(struct command *c);
int d_trance(struct command *c);
int v_create_iron_golem(struct command *c);
int v_teleport_item(struct command *c);
int v_trance(struct command *c);

/* art.c */
int d_conceal_arts(struct command *c);
int d_destroy_art(struct command *c);
int d_detect_arts(struct command *c);
int d_forge_art_x(struct command *c);
int d_forge_aura(struct command *c);
int d_forge_palantir(struct command *c);
int d_mutate_art(struct command *c);
int d_obscure_art(struct command *c);
int d_reveal_arts(struct command *c);
int d_unobscure_art(struct command *c);
int d_use_palantir(struct command *c);
int new_suffuse_ring(int who);
int v_conceal_arts(struct command *c);
int v_deep_identify(struct command *c);
int v_destroy_art(struct command *c);
int v_detect_arts(struct command *c);
int v_forge_art_x(struct command *c);
int v_forge_aura(struct command *c);
int v_forge_palantir(struct command *c);
int v_mutate_art(struct command *c);
int v_obscure_art(struct command *c);
int v_reveal_arts(struct command *c);
int v_unobscure_art(struct command *c);
int v_use_orb(struct command *c);
int v_use_palantir(struct command *c);

/* artifacts.c */
int artifact_identify(const char *header, struct command *c);
int best_artifact(int who, int type, int param2, int uses);
int combat_artifact_bonus(int who, int part, int *item_used);
int create_random_artifact(int monster);
int create_specific_artifact(int monster, int t);
int effective_workers(int who);
int has_artifact(int who, int type, int p1, int p2, int charges);
int random_beast(int sk);
int v_art_crown(struct command *c);
int v_art_destroy(struct command *c);
int v_art_orb(struct command *c);
int v_art_teleport(struct command *c);
int v_identify(struct command *c);
int v_make_artifact(struct command *c);
int v_power_jewel(struct command *c);
int v_summon_aid(struct command *c);

/* beast.c */
int d_bird_spy(struct command *c);
int d_breed(struct command *c);
int d_capture_beasts(struct command *c);
int v_bird_spy(struct command *c);
int v_breed(struct command *c);
int v_capture_beasts(struct command *c);

/* buy.c */
void add_city_trade(int where, int kind, int item, int qty, int cost, int month);
void add_scrolls(int where);
void check_validated_trades(void);
void clear_all_trades(int who);
void delete_city_trade(int where, int item);
struct trade *find_trade(int who, int kind, int item);
void investigate_possible_trade(int who, int item, int old_has);
void list_pending_trades(int who, int num);
void market_report(int who, int where);
int market_here(int who);
void match_all_trades(void);
void match_trades(int who);
struct trade *new_trade(int who, int kind, int item);
void opium_market_delta(int where);
void trade_suffuse_ring(int where);
void update_city_trade(int where, int kind, int item, int qty, int cost, int month);
int v_buy(struct command *c);
int v_sell(struct command *c);

/* build.c */
void connect_locations(int loc1, int dir1, int loc2, int dir2);
int d_build(struct command *c);
int d_fortify_castle(struct command *c);
int d_moat_castle(struct command *c);
int d_raze(struct command *c);
int d_repair(struct command *c);
int d_strengthen_castle(struct command *c);
int d_widen_entrance(struct command *c);
int fort_default_defense(int sk);
struct entity_build *get_build(int what, int t);
int i_repair(struct command *c);
int province_subloc(int where, int sk);
int real_orc_loc_okay(int who, int where);
void unconnect_location(int loc1);
int v_build(struct command *c);
int v_fortify_castle(struct command *c);
int v_moat_castle(struct command *c);
int v_raze(struct command *c);
int v_repair(struct command *c);
int v_strengthen_castle(struct command *c);
int v_widen_entrance(struct command *c);

/* basic.c */
void add_aura(int who, int aura);
int d_adv_med(struct command *c);
int d_create_dirt_golem(struct command *c);
int d_detect_abil(struct command *c);
int d_dispel_abil(struct command *c);
int d_hinder_med(struct command *c);
int d_mage_menial(struct command *c);
int d_meditate(struct command *c);
int d_quick_cast(struct command *c);
int d_reveal_mage(struct command *c);
int d_save_quick(struct command *c);
int d_shroud_abil(struct command *c);
int d_tap_health(struct command *c);
int d_view_aura(struct command *c);
int d_write_spell(struct command *c);
int hinder_med_chance(int who);
void hinder_med_omen(int who, int other);
int v_adv_med(struct command *c);
int v_appear_common(struct command *c);
int v_create_dirt_golem(struct command *c);
int v_detect_abil(struct command *c);
int v_dispel_abil(struct command *c);
int v_hinder_med(struct command *c);
int v_mage_menial(struct command *c);
int v_meditate(struct command *c);
int v_quick_cast(struct command *c);
int v_reveal_mage(struct command *c);
int v_save_quick(struct command *c);
int v_shroud_abil(struct command *c);
int v_tap_health(struct command *c);
int v_use_quick_cast(struct command *c);
int v_view_aura(struct command *c);
int v_write_spell(struct command *c);

/* c2.c */
int d_archery(struct command *c);
int d_dedicate(struct command *c);
int d_defense(struct command *c);
int d_improve_opium(struct command *c);
int d_swordplay(struct command *c);
int line_length_check(cstrings_list l);
cstrings_list parse_text_list(struct command *c);
void text_list_free(cstrings_list l);
int v_archery(struct command *c);
int v_board(struct command *c);
int v_border(struct command *c);
int v_claim(struct command *c);
int v_dedicate(struct command *c);
int v_defense(struct command *c);
int v_die(struct command *c);
int v_discard(struct command *c);
int v_fee(struct command *c);
int v_ferry(struct command *c);
int v_fight_to_death(struct command *c);
int v_improve_opium(struct command *c);
int v_message(struct command *c);
int v_post(struct command *c);
int v_quit(struct command *c);
int v_stop(struct command *c);
int v_swordplay(struct command *c);
int v_tax(struct command *c);
int v_tell(struct command *c);
int v_unload(struct command *c);

/* check.c */
void check_db(void);

/* cloud.c */
void create_cloudlands(void);
void float_cloudlands(void);

/* c1.c */
void clear_wait_parse(struct command *c);
int d_explore(struct command *c);
int d_form(struct command *c);
int d_wait(struct command *c);
int how_many(int who, int from_who, int item, int qty, int have_left);
int i_wait(struct command *c);
int next_np_turn(int pl);
char *parse_wait_args(struct command *c);
void print_hiring_status(int pl);
void print_unformed(int pl);
int v_accept(struct command *c);
int v_banner(struct command *c);
int v_emote(struct command *c);
int v_explore(struct command *c);
int v_flag(struct command *c);
int v_form(struct command *c);
int v_fullname(struct command *c);
int v_get(struct command *c);
int v_give(struct command *c);
int v_look(struct command *c);
int v_name(struct command *c);
int v_pay(struct command *c);
int v_split(struct command *c);
int v_times(struct command *c);
int v_wait(struct command *c);
int will_accept(int who, int item, int from, int qty);

/* code.c */
void alloc_box(int n, int kind, int sk);
char *box_code(int n);
char *box_code_less(int n);
char *box_name(int n);
char *box_name_kind(int n);
char *box_name_qty(int item, int qty);
void change_box_kind(int n, int kind);
void change_box_subkind(int n, int sk);
int code_to_int(const char *s);
void delete_box(int n);
const char *just_name(int n);
char *just_name_qty(int item, int qty);
const char *name(int n);
int new_ent(int kind, int sk);
char *plural_item_box(int item, int qty);
const char *plural_item_name(int item, int qty);
int scode(char *s);
void set_banner(int n, char *s);
void set_name(int n, const char *s);

/* combat.c */
int calc_beast_limit(int who, int defender);
int calc_man_limit(int who, int defender);
void check_all_auto_attacks(void);
void clear_second_waits(void);
int combat_comp(const void *a_ptr, const void *b_ptr);
int d_pillage(struct command *c);
int do_trap_attack(int who, int num_attacks, int chance);
int loc_target(int target);
int regular_combat(int a, int b, int seize_slot, int already_waiting);
int select_attacker(int who, int target);
extern int show_combat_flag;
int v_attack_tactics(struct command *c);
int v_behind(struct command *c);
int v_defense_tactics(struct command *c);
int v_execute(struct command *c);
int v_guard(struct command *c);
int v_pillage(struct command *c);
int v_prac_control(struct command *c);
int v_prac_protect(struct command *c);
int v_use_beasts(struct command *c);

/* day.c */
void add_unformed_sup(int pl);
void daily_events(void);
int has_city(int where);
void init_locs_touched(void);
int near_rocky_coast(int where);
void open_faery_hill(int where);
int place_here(int where, int who);
void post_month(void);
void post_production(void);
void pre_month(void);

/* dir.c */
void determine_map_edges(void);
void dir_assert(void);
int is_port_city(int where);
int los_province_distance(int a, int b);
int rc_to_region(int row, int col);
int region_col(int where);
int region_row(int where);

/* display.c */
char *deserted_s(int n);
char *display_owner(int who);
char *display_with(int who);
char *liner_desc(int n);
char *nation_s(int n);
void show_chars_below(int who, int n);
void show_loc(int who, int where);
char *show_loc_header(int where);
void show_loc_posts(int who, int where, int show_full_loc);
void show_owner_stack(int who, int n);

/* eat.c */
char *accept_comment(struct command *c);
void admit_check(struct command *c);
char *admit_comment(struct command *c);
char *attack_comment(struct command *c);
char *attitude_comment(struct command *c);
char *buy_comment(struct command *c);
char *catch_comment(struct command *c);
char *collect_comment(struct command *c);
char *default_comment(struct command *c);
char *drop_comment(struct command *c);
void eat_loop(int mail_now);
char *get_comment(struct command *c);
char *give_comment(struct command *c);
char *move_comment(struct command *c);
void quit_check(struct command *c);
int read_spool(int mail_now);
char *study_comment(struct command *c);
int v_format(struct command *c);
int v_notab(struct command *c);

/* effect.c */
int add_effect(int what, int t, int st, int duration, int value);
void delete_effect(int what, int type, int st);
void delete_all_effects(int what, int type, int st);
int get_all_effects(int what, int t, int st, int v);
int get_effect(int what, int t, int st, int v);
void update_effects(int what);

/* faery.c */
void create_faery(void);
void do_wild_hunt(int where);
void update_faery(void);
int v_use_faery_artifact(struct command *c);

/* hero.c */
int v_forced_march(struct command *c);
int v_personal_fight_to_death(struct command *c);

/* garr.c */
void determine_noble_ranks(void);
int garrison_here(int where);
int garrison_notices(int garr, int target);
int garrison_spot_check(int garr, int target);
void garrison_summary(int pl);
int may_rule_here(int who, int where);
void ping_garrisons(void);
int province_admin(int n);
char *rank_s(int who);
int top_ruler(int n);
int v_decree(struct command *c);
int v_decree_hostile(struct command *c);
int v_decree_watch(struct command *c);
int v_garrison(struct command *c);
int v_ungarrison(struct command *c);

/* gate.c */
int d_detect_gates(struct command *c);
int d_notify_jump(struct command *c);
int d_notify_unseal(struct command *c);
int d_rem_seal(struct command *c);
int d_reveal_key(struct command *c);
int d_seal_gate(struct command *c);
int d_unseal_gate(struct command *c);
void do_jump(int who, int dest, int gate, int backwards);
int list_gates_here(int who, int where, int show_dest);
int province_gate_here(int where);
int v_detect_gates(struct command *c);
int v_jump_gate(struct command *c);
int v_notify_jump(struct command *c);
int v_notify_unseal(struct command *c);
int v_rem_seal(struct command *c);
int v_reveal_key(struct command *c);
int v_reverse_jump(struct command *c);
int v_seal_gate(struct command *c);
int v_teleport(struct command *c);
int v_unseal_gate(struct command *c);

/* glob.c */
void glob_init(void);

/* gm.c */
void gm_report(int pl);
void list_all_items(int pl);
void list_all_notices(int pl);

/* hades.c */
void auto_hades(void);
void create_hades(void);

/* immed.c */
void immediate_commands(void);
void fix_gates(void);
int v_add_item(struct command *c);
int v_be(struct command *c);
int v_ct(struct command *c);
int v_credit(struct command *c);
int v_dump(struct command *c);
int v_fix(struct command *c);
int v_fix2(struct command *c);
int v_invent(struct command *c);
int v_kill(struct command *c);
int v_know(struct command *c);
int v_listcmds(struct command *c);
int v_lore(struct command *c);
int v_los(struct command *c);
int v_makeloc(struct command *c);
int v_poof(struct command *c);
int v_postproc(struct command *c);
int v_relore(struct command *c);
int v_save(struct command *c);
int v_see_all(struct command *c);
int v_seed(struct command *c);
int v_seedmarket(struct command *c);
int v_skills(struct command *c);
int v_sub_item(struct command *c);
int v_take_pris(struct command *c);
int v_xyzzy(struct command *c);

/* input.c */
void check_all_waits(void);
int check_allow(struct command *c, char *allow);
void cmd_shift(struct command *c);
void command_done(struct command *c);
void do_command(struct command *c);
void do_urchin_spies(struct command *c);
int find_command(char *s);
int finish_command(struct command *c);
void init_load_sup(int who);
void init_wait_list(void);
void initial_command_load(void);
int min_pri_ready(void);
int oly_parse(struct command *c, char *s);
int oly_parse_cmd(struct command *c, char *s);
int parse_arg(int who, char *s);
void process_orders(void);
void remove_comment(char *s);
void remove_ctrl_chars(char *s);
void start_phase(void);

/* io.c */
void load_db(void);
void save_box(FILE *fp, int n);
void save_db(void);
void save_logdir(void);
void write_player(int pl);

/* kill.c */
int nearby_grave(int where);

/* loc.c */
void add_to_here_list(int loc, int who);
void all_char_here(int who, ilist *l);
void all_here(int who, ilist *l);
void all_stack(int who, ilist *l);
int building_owner(int where);
int count_loc_structures(int where, int a, int b);
int in_here_list(int loc, int who);
int in_safe_now(int who);
int province(int who);
void remove_from_here_list(int loc, int who);
int region(int who);
void set_where(int who, int new_loc);
int somewhere_inside(int a, int b);
int subloc(int where);
int subloc_here(int where, int sk);
void mark_loc_stack_known(int stack, int where);
int viewloc(int who);

/* lore.c */
void deliver_lore(int who, int num);
void gm_show_all_skills(int pl, int use_texi);
char *np_req_s(int skill);
void queue_lore(int who, int num, int anyway);
void scan_char_item_lore(void);
void scan_char_skill_lore(void);
void show_lore_sheets(void);

/* main.c */
int send_rep(int pl, int turn);
int v_nationlist(struct command *c);

/* make.c */
int d_make(struct command *c);
int i_make(struct command *c);
int v_make(struct command *c);
int v_use_train_riding(struct command *c);
int v_use_train_war(struct command *c);

/* map.c */
int load_cmap(void);
int load_cmap_players(void);
void print_map(FILE *fp);

/* mining.c */
int d_add_iron_shoring(struct command *c);
int d_add_wooden_shoring(struct command *c);
int d_mine_gate_crystal(struct command *c);
int d_mine_gold(struct command *c);
int d_mine_iron(struct command *c);
int d_mine_mithril(struct command *c);
void create_mine_info(int mine);
struct entity_mine *get_mine_info(int where);
int mine_depth(int where);
int v_add_iron_shoring(struct command *c);
int v_add_wooden_shoring(struct command *c);
int v_mine_gate_crystal(struct command *c);
int v_mine_gold(struct command *c);
int v_mine_iron(struct command *c);
int v_mine_mithril(struct command *c);

/* move.c */
int calc_entrance_fee(struct loc_control_ent *control, struct command *c, int ruler);
int can_join_guild(int who, int g);
int charge_entrance_fees(int who, int ruler, int cost);
void check_arrival_effects(int who, int where, int flying);
void check_captain_loses_sailors(int qty, int target, int inform);
int check_peaceful_move(struct command *c, struct exit_view *v);
void check_ocean_chars(void);
void clear_guard_flag(int who);
int controls_loc(int where);
int d_fly(struct command *c);
int d_move_attack(struct command *c);
int d_sail(struct command *c);
void init_ocean_chars(void);
int i_sail(struct command *c);
int join_guild(int who, int g);
int move_permitted(struct command *c, struct exit_view *v);
void move_stack(int who, int where);
int peaceful_enter(int who, int where, int target);
int player_controls_loc(int where);
void restore_stack_actions(int who);
void restore_v_array(struct command *c, struct exit_view *v);
void touch_loc_after_move(int who, int where);
int v_east(struct command *c);
int v_enter(struct command *c);
int v_exit(struct command *c);
int v_fly(struct command *c);
int v_move_attack(struct command *c);
int v_north(struct command *c);
int v_maxpay(struct command *c);
int v_sail(struct command *c);
int v_south(struct command *c);
int v_west(struct command *c);

/* necro.c */
void auto_undead(int who);
int d_aura_blast(struct command *c);
int d_banish_undead(struct command *c);
int d_create_flesh_golem(struct command *c);
int d_eat_dead(struct command *c);
int d_exhume(struct command *c);
int d_keep_undead(struct command *c);
int d_undead_lord(struct command *c);
int v_aura_blast(struct command *c);
int v_aura_reflect(struct command *c);
int v_banish_undead(struct command *c);
int v_create_flesh_golem(struct command *c);
int v_eat_dead(struct command *c);
int v_exhume(struct command *c);
int v_keep_undead(struct command *c);
int v_undead_lord(struct command *c);

/* npc.c */
int controlled_humans_here(int where);
int create_monster_stack(int selection, int total, int where);
int create_new_beasts(int where, int sk);
int create_peasant_mob(int where);
int d_npc_breed(struct command *c);
int do_cookie_npc(int who, int where, int cookie, int place);
void do_npc_orders(int who, int flush, int queue_orders);
int may_cookie_npc(int who, int where, int cookie);
void npc_move(int who);
void queue_npc_orders(void);
int v_think(struct command *c);
void wilderness_attack_check(int who, int where);

/* order.c */
void flush_unit_orders(int pl, int who);
void list_order_templates(void);
void load_orders(void);
void orders_template(int who, int pl);
void pop_order(int pl, int who);
void prepend_order(int pl, int who, char *s);
void queue(int who, const char *s, ...);
void queue_order(int pl, int who, char *s);
void queue_stop(int pl, int who);
void save_orders(void);
void save_player_orders(int pl);
int stop_order(int pl, int who);
char *top_order(int pl, int who);

/* perm.c */
void clear_all_att(int who);
void clear_att(int who, int disp);
int find_nation(const char *name);
int is_defend(int who, int targ);
int is_hostile(int who, int targ);
int nation(int who);
void print_admit(int pl);
void print_att(int who, int n);
void set_att(int who, int targ, int disp);
int v_admit(struct command *c);
int v_att_clear(struct command *c);
int v_defend(struct command *c);
int v_hostile(struct command *c);
int v_neutral(struct command *c);
int will_admit(int pl, int who, int targ);

/* produce.c */
int d_collect(struct command *c);
int d_grow_pop(struct command *c);
int i_collect(struct command *c);
void init_collect_list(void);
void location_production(void);
int v_catch(struct command *c);
int v_collect(struct command *c);
int v_fish(struct command *c);
int v_grow_pop(struct command *c);
int v_mallorn(struct command *c);
int v_opium(struct command *c);
int v_quarry(struct command *c);
int v_raise_corpses(struct command *c);
int v_recruit(struct command *c);
int v_wood(struct command *c);
int v_yew(struct command *c);

/* quest.c */
void generate_one_treasure(int monster);
void generate_treasure(int unit, int divisor);
long MM(int item);
void reseed_monster_sublocs(void);
void seed_monster_sublocs(void);

/* ranger.c */
int d_find_food(struct command *c);
int v_find_food(struct command *c);

/* relig.c */
int add_piety(int who, int amount, int use_limit);
int check_loc_for_beasts(int where, int who);
int d_arrange_mugging(struct command *c);
int d_bless_fort(struct command *c);
int d_calm_peasants(struct command *c);
int d_conceal_location(struct command *c);
int d_conceal_mine(struct command *c);
int d_create_holy_symbol(struct command *c);
int d_create_mist(struct command *c);
int d_create_mithril(struct command *c);
int d_create_ninja(struct command *c);
int d_dedicate_temple(struct command *c);
int d_detect_beasts(struct command *c);
int d_draw_crowds(struct command *c);
int d_edge_of_kireus(struct command *c);
int d_enchant_guard(struct command *c);
int d_find_all_hidden_features(struct command *c);
int d_find_hidden_features(struct command *c);
int d_gather_holy_plant(struct command *c);
int d_generic_trap(struct command *c);
int d_guard_loyalty(struct command *c);
int d_heal(struct command *c);
int d_hinder_med_b(struct command *c);
int d_improve_charisma(struct command *c);
int d_improve_fort(struct command *c);
int d_improve_logging(struct command *c);
int d_improve_mining(struct command *c);
int d_improve_quarrying(struct command *c);
int d_improve_smithing(struct command *c);
int d_improve_taxes(struct command *c);
int d_instill_fanaticism(struct command *c);
int d_last_rites(struct command *c);
int d_mesmerize_crowd(struct command *c);
int d_obscure_forest_trail(struct command *c);
int d_obscure_mountain_trail(struct command *c);
int d_prep_ritual(struct command *c);
int d_protect_mine(struct command *c);
int d_recruit_elves(struct command *c);
int d_remove_bless(struct command *c);
int d_resurrect(struct command *c);
int d_reveal_vision(struct command *c);
int d_urchin_spy(struct command *c);
int d_vision_reg(struct command *c);
int d_weaken_fort(struct command *c);
int has_holy_plant(int who);
int has_holy_symbol(int who);
int has_piety(int who, int amount);
int is_holy_terrain(int n, int where);
int is_member(int who, int relig);
int priest_in_stack(int who, int which);
int use_piety(int who, int amount);
int v_arrange_mugging(struct command *c);
int v_bless_follower(struct command *c);
int v_bless_fort(struct command *c);
int v_conceal_location(struct command *c);
int v_conceal_mine(struct command *c);
int v_create_holy_symbol(struct command *c);
int v_create_mithril(struct command *c);
int v_create_ninja(struct command *c);
int v_dedicate_temple(struct command *c);
int v_detect_beasts(struct command *c);
int v_draw_crowds(struct command *c);
int v_edge_of_kireus(struct command *c);
int v_enchant_guard(struct command *c);
int v_find_forest_trail(struct command *c);
int v_find_hidden_features(struct command *c);
int v_find_mountain_trail(struct command *c);
int v_gather_holy_plant(struct command *c);
int v_generic_trap(struct command *c);
int v_heal(struct command *c);
int v_hinder_med_b(struct command *c);
int v_improve_charisma(struct command *c);
int v_improve_fort(struct command *c);
int v_improve_logging(struct command *c);
int v_improve_mining(struct command *c);
int v_improve_quarrying(struct command *c);
int v_improve_smithing(struct command *c);
int v_instill_fanaticism(struct command *c);
int v_last_rites(struct command *c);
int v_obscure_forest_trail(struct command *c);
int v_obscure_mountain_trail(struct command *c);
int v_prep_ritual(struct command *c);
int v_proselytise(struct command *c);
int v_protect_mine(struct command *c);
int v_recruit_elves(struct command *c);
int v_remove_bless(struct command *c);
int v_resurrect(struct command *c);
int v_reveal_vision(struct command *c);
int v_urchin_spy(struct command *c);
int v_vision_reg(struct command *c);
int v_weaken_fort(struct command *c);

/* report.c */
void char_rep_health(int who, int num, char *prefix);
char *char_rep_location(int who);
void char_rep_magic(int who, int num, char *prefix);
void char_rep_sup(int who, int num);
void character_report(void);
void charge_account(void);
void determine_output_order(void);
char *extra_item_info(int who, int item, int qty);
void gen_include_section(void);
void gen_include_sup(int pl);
int inv_item_comp(const void *a_ptr, const void *b_ptr);
void player_banner(void);
void player_ent_info(void);
void player_report(void);
void player_report_sup(int pl);
void report_account(void);
int report_account_out(int pl, int who);
void report_account_sup(int pl);
void show_carry_capacity(int who, int num);
void show_char_inventory(int who, int num, char *prefix);
void show_item_skills(int who, int num);
void show_unclaimed(int who, int num);
void sort_for_output(ilist l);
void turn_end_loc_reports(void);

/* scry.c */
void alert_palantir_scry(int who, int where);
void alert_scry_attempt(int who, int where, char *t);
void alert_scry_generic(int who, int where);
int cast_check_char_here(int who, int target);
int cast_where(int who);
int d_bar_loc(struct command *c);
int d_detect_scry(struct command *c);
int d_dispel_region(struct command *c);
int d_locate_char(struct command *c);
int d_proj_cast(struct command *c);
int d_save_proj(struct command *c);
int d_scry_region(struct command *c);
int d_shroud_region(struct command *c);
int d_unbar_loc(struct command *c);
void notify_loc_shroud(int where);
int reset_cast_where(int who);
void show_item_where(int who, int target);
int v_bar_loc(struct command *c);
int v_detect_scry(struct command *c);
int v_dispel_region(struct command *c);
int v_locate_char(struct command *c);
int v_proj_cast(struct command *c);
int v_save_proj(struct command *c);
int v_scry_region(struct command *c);
int v_shroud_region(struct command *c);
int v_unbar_loc(struct command *c);
int v_use_proj_cast(struct command *c);

/* savage.c */
void auto_savage(int who);
int d_keep_savage(struct command *c);
void init_savage_attacks(void);
int savage_hates(int where);
int v_keep_savage(struct command *c);
int v_summon_savage(struct command *c);
int v_use_drum(struct command *c);

/* seed.c */
void compute_dist(void);
int int_comp(const void *a, const void *b);
void seed_city(int where);
void seed_city_near_lists(void);
void seed_city_trade(int where);
void seed_common_tradegoods(void);
void seed_cookies(void);
void seed_initial_locations(void);
void seed_orcs(void);
void seed_population(void);
void seed_rare_tradegoods(void);
void seed_taxes(void);
int v_seedorc(struct command *c);

/* shipcraft.c */
int d_add_forts(struct command *c);
int d_add_keels(struct command *c);
int d_add_ports(struct command *c);
int d_add_ram(struct command *c);
int d_add_sails(struct command *c);
int d_remove_forts(struct command *c);
int d_remove_keels(struct command *c);
int d_remove_ports(struct command *c);
int d_remove_ram(struct command *c);
int d_remove_sails(struct command *c);
void ship_storm_check(int ship);
void ship_summary(int pl);
int v_add_forts(struct command *c);
int v_add_keels(struct command *c);
int v_add_ports(struct command *c);
int v_add_ram(struct command *c);
int v_add_sails(struct command *c);
int v_remove_forts(struct command *c);
int v_remove_keels(struct command *c);
int v_remove_ports(struct command *c);
int v_remove_ram(struct command *c);
int v_remove_sails(struct command *c);

/* sout.c */
const char *comma_append(const char *s, const char *t);
void close_logfile(void);
void html(int who, const char *format, ...) __attribute__((format(printf, 2, 3)));
void init_spaces(void);
void initialize_buffer(void);
void lines(int who, char *s);
void log_output(int k, const char *format, ...) __attribute__((format(printf, 2, 3)));
void match_lines(int who, char *s);
void open_logfile(void);
void open_logfile_nondestruct(void);
void out(int who, const char *format, ...) __attribute__((format(printf, 2, 3)));
void restore_output_vector(ilist t);
char *sout(const char *format, ...) __attribute__((format(printf, 1, 2)));
ilist save_output_vector(void);
void tags_off(void);
void tags_on(void);
void tagout(int who, const char *format, ...) __attribute__((format(printf, 2, 3)));
void vector_add(int who);
void vector_char_here(int where);
void vector_clear(void);
void vector_players(void);
void vector_stack(int who, int clear);
void wiout(int who, int ind, const char *format, ...) __attribute__((format(printf, 3, 4)));
void wout(int who, const char *format, ...) __attribute__((format(printf, 2, 3)));
void wrap_set(int who);
void wrap_done(void);

/* stack.c */
int check_prisoner_escape(int who, int chance);
int contains_mu_undead(int i);
void drop_stack(int who, int to_drop);
void extract_stacked_unit(int who);
int give_prisoner(int who, int target, int pris);
int has_prisoner(int who, int pris);
int here_pos(int who);
void join_stack(int who, int target);
void leave_stack(int who);
int move_prisoner(int who, int target, int pris);
void prisoner_movement_escape_check(int who);
void promote(int who, int new_pos);
void promote_stack(int lower, int higher);
int stack_contains_mu_undead(int who);
int stack_contains_priest(int who);
int stack_leader(int who);
int stack_parent(int who);
int stacked_beneath(int a, int b);
void take_prisoner(int who, int target);
int v_promote(struct command *c);
int v_stack(struct command *c);
int v_surrender(struct command *c);
int v_unstack(struct command *c);
void weekly_prisoner_escape_check(void);

/* stealth.c */
void clear_contacts(int stack);
int cloak_lord(int n);
int d_assassinate(struct command *c);
int d_conceal_nation(struct command *c);
int d_find_rich(struct command *c);
int d_hide(struct command *c);
int d_petty_thief(struct command *c);
int d_seek(struct command *c);
int d_sneak(struct command *c);
int d_spy_inv(struct command *c);
int d_spy_lord(struct command *c);
int d_spy_skills(struct command *c);
int d_torture(struct command *c);
int v_assassinate(struct command *c);
int v_conceal_nation(struct command *c);
int v_contact(struct command *c);
int v_find_rich(struct command *c);
int v_hide(struct command *c);
int v_petty_thief(struct command *c);
int v_seek(struct command *c);
int v_sneak(struct command *c);
int v_spy_inv(struct command *c);
int v_spy_lord(struct command *c);
int v_spy_skills(struct command *c);
int v_torture(struct command *c);

/* storm.c */
int can_see_weather_here(int who, int where);
int d_banish_corpses(struct command *c);
int d_bind_storm(struct command *c);
int d_death_fog(struct command *c);
int d_direct_storm(struct command *c);
int d_dissipate(struct command *c);
int d_fierce_wind(struct command *c);
int d_lightning(struct command *c);
int d_list_storms(struct command *c);
int d_renew_storm(struct command *c);
int d_seize_storm(struct command *c);
int d_summon_fog(struct command *c);
int d_summon_rain(struct command *c);
int d_summon_wind(struct command *c);
void dissipate_storm(int storm, int show);
void init_weather_views(void);
void natural_weather(void);
int new_storm(int new, int sk, int aura, int where);
void storm_report(int pl);
void update_weather_view_locs(int stack, int where);
int v_banish_corpses(struct command *c);
int v_bind_storm(struct command *c);
int v_death_fog(struct command *c);
int v_direct_storm(struct command *c);
int v_dissipate(struct command *c);
int v_fierce_wind(struct command *c);
int v_lightning(struct command *c);
int v_list_storms(struct command *c);
int v_renew_storm(struct command *c);
int v_seize_storm(struct command *c);
int v_summon_fog(struct command *c);
int v_summon_rain(struct command *c);
int v_summon_wind(struct command *c);

/* summary.c */
void summary_report(void);

/* swear.c */
int d_bribe(struct command *c);
int d_incite(struct command *c);
int d_persuade_oath(struct command *c);
int d_raise(struct command *c);
int d_rally(struct command *c);
int d_terrorize(struct command *c);
int v_bribe(struct command *c);
int v_honor(struct command *c);
int v_incite(struct command *c);
int v_oath(struct command *c);
int v_persuade_oath(struct command *c);
int v_raise(struct command *c);
int v_rally(struct command *c);
int v_swear(struct command *c);
int v_terrorize(struct command *c);

/* times.c */
void do_times(void);
void open_times(void);
int v_press(struct command *c);
int v_rumor(struct command *c);

/* trading.c */
int d_decrease_demand(struct command *c);
int d_decrease_supply(struct command *c);
int d_dedicate_tower(struct command *c);
int d_hide_item(struct command *c);
int d_hide_money(struct command *c);
int d_increase_demand(struct command *c);
int d_increase_supply(struct command *c);
int d_smuggle_goods(struct command *c);
int d_smuggle_men(struct command *c);
void do_production(int where, int override);
void make_tower_guild(int where, int school);
int random_trade_good(void);
int v_decrease_demand(struct command *c);
int v_decrease_supply(struct command *c);
int v_dedicate_tower(struct command *c);
int v_hide_item(struct command *c);
int v_hide_money(struct command *c);
int v_increase_demand(struct command *c);
int v_increase_supply(struct command *c);
int v_smuggle_goods(struct command *c);
int v_smuggle_men(struct command *c);
void update_markets(void);

/* u.c */
int autocharge(int who, int amount);
int beast_wild(int who);
int check_skill(int who, int skill);
int test_bit(ilist kr, int i);
int char_here(int who, int target);
void char_reclaim(int who);
int contacted(int a, int b);
int count_any(int who);
int count_any_real(int who, int ignore_ninjas, int ignore_angels);
int count_fighters(int who, int attack_min);
int count_generic(int who, int stack, int (*func)(int));
int count_stack_any(int who);
int count_stack_any_real(int who, int ignore_ninjas, int ignore_angels);
int crosses_ocean(int a, int b);
int first_character(int where);
void get_rid_of_collapsed_mine(int fort);
int item_fly_cap(int who);
int item_land_cap(int who);
int item_ride_cap(int who);
int item_weight(int who);
int loc_contains_hidden(int n);
int loc_depth(int n);
void lock_tag(void);
void print_dot(int c);
void put_back_cookie(int who);
void set_bit(int **kr, int i);
int ship_cap(int ship);
void stage(const char *s);
int sub_item(int who, int item, int qty);
int survive_fatal(int who);
int v_reclaim(struct command *c);

const char *cap(const char *s);
char *comma_num(int n);
char *gold_s(int n);
char *knum(int n, int nozero);
char *loyal_s(int who);
char *more_weeks(int n);
char *nice_num(int n);
char *ordinal(int n);
char *rest_name(struct command *c, int a);
char *weeks(int n);
int add_structure_damage(int fort, int damage);
int beast_capturable(int who);
int can_pay(int who, int amount);
int charge(int who, int amount);
int charge_aura(int who, int amount);
int check_aura(int who, int amount);
int check_char_gone(int who, int target);
int check_char_here(int who, int target);
int check_still_here(int who, int target);
int consume_item(int who, int item, int qty);
int count_loc_char_item(int where, int item);
int count_man_items(int who);
int count_stack_figures(int who);
int count_stack_units(int who);
int create_unique_item(int who, int sk);
int deduct_aura(int who, int amount);
int deduct_np(int pl, int num);
int drop_item(int who, int item, int qty);
int find_nearest_land(int where);
int first_char_here(int where);
int has_item(int who, int item);
int has_use_key(int who, int key);
int loc_hidden(int n);
int lookup(char *table[], char *s);
int max(int a, int b);
int min(int a, int b);
int move_item(int from, int to, int item, int qty);
int my_prisoner(int who, int pris);
int my_sqrt(int n);
int mylog(int base, int num);
int new_char(int, int, int, int, int, int, int, const char *);
int nprovinces(void);
int ship_weight(int ship);
int stack_has_item(int who, int item);
int stack_has_use_key(int who, int key);
int stackmate_inheritor(int who);
int test_known(int who, int i);
void add_char_damage(int who, int amount, int inherit);
void add_np(int pl, int num);
void clear_temps(int kind);
void delta_loyalty(int who, int amount, int silent);
void destroy_unique_item(int who, int item);
void determine_stack_weights(int who, struct weights *w, int mountains);
void determine_unit_weights(int who, struct weights *w, int mountains);
void gen_item(int who, int item, int qty);
void get_rid_of_building(int fort);
void hack_unique_item(int item, int owner);
void kill_char(int who, int inherit, int dont_bury);
void kill_stack_ocean(int who);
void set_known(int who, int i);
void take_unit_items(int from, int inherit, int how_many);

/* use.c */
void add_skill_experience(int who, int sk);
int alone_here(int who);
void artifact_use_speedup(struct command *c);
int being_taught(int who, int sk, int *item, int *teach_bonus);
void check_skill_times(void);
int d_practice(struct command *c);
int d_research(struct command *c);
int d_study(struct command *c);
int d_use(struct command *c);
int d_use_item(struct command *c);
void experience_use_speedup(struct command *c);
char *exp_s(int level);
int find_use_entry(int skill);
int forget_skill(int who, int skill);
int has_skill(int who, int skill);
int has_subskill(int who, int subskill);
int i_use(struct command *c);
int learn_skill(int who, int sk);
void list_partial_skills(int who, int num, char *prefix);
void list_skill_sup(int who, struct skill_ent *e, const char *prefix);
void list_skills(int who, int num, char *prefix);
struct skill_ent *p_skill_ent(int who, int skill);
void religion_use_speedup(struct command *c);
struct skill_ent *rp_skill_ent(int who, int skill);
void set_skill(int who, int skill, int know);
int skill_cost(int sk);
int skill_school(int sk);
void use_studypoint(int who);
int v_acquire(struct command *c);
int v_forget(struct command *c);
int v_implicit(struct command *c);
int v_practice(struct command *c);
int v_research(struct command *c);
int v_shipbuild(struct command *c);
int v_study(struct command *c);
int v_teach(struct command *c);
int v_use(struct command *c);
int v_use_cs(struct command *c);
int v_use_item(struct command *c);
int v_use_special_staff(struct command *c);

/* win.c */
void check_win_conditions(void);

/* z.c */
void asfail(char *file, int line, char *cond);
void copy_fp(FILE *a, FILE *b);
char *eat_leading_trailing_whitespace(char *s);
int fuzzy_strcmp(const char *one, const char *two);
char *getlin(FILE *fp);
char *getlin_ew(FILE *fp);
void init_lower(void);
int i_strcmp(const char *s, const char *t);
int i_strncmp(char *s, char *t, int n);
void init_random(void);
void lcase(char *s);
int readfile(char *path);
char *readlin(void);
char *readlin_ew(void);
int rnd(int low, int high);
char *str_save(const char *s);
void test_random(void);

#endif //OLYMPIA_LEGACY_H
