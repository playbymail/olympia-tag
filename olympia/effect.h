/*
 *  EFFECT.H
 *  Tue Aug  6 12:09:42 1996 -- Scott Turner
 *
 */
int get_all_effects(int what, int t, int st, int v);
int get_effect(int what, int t, int st, int v);
void update_effects(int what);
int add_effect(int what, int t, int st, int duration, int value);
void delete_effect(int what, int type, int st);

/*
 *  Loop over the effects on something.
 *
 *  What = int
 *  e = struct effect *;
 *
  if (rp_char(what)) { \
    el =  rp_char(what)->effects; \
  } else if (rp_loc(what)) { \
    el =  rp_loc(what)->effects; \
  } else if (rp_subloc(what)) { \
    el =  rp_subloc(what)->effects; \
  }; \
 * 
 */
#define	loop_effects(what, e) \
{ int ll_i; \
  effects_list ll_l = NULL, el = NULL; \
  int ll_check = 29; \
  el = effects(what); \
  ll_l = effects_copy(el); \
  for (ll_i = 0; ll_i < effects_len(ll_l); ll_i++) {  \
    e = ll_l[ll_i];

#define	next_effect	} assert(ll_check == 29); effects_reclaim(&ll_l); }
