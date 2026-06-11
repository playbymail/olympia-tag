
int loc_owner(int where);
void mark_loc_stack_known(int stack, int where);

void all_here(int who, ilist *l);
void all_char_here(int who, ilist *l);

int region(int who);
int province(int who);
int subloc(int who);
int viewloc(int who);

void add_to_here_list(int loc, int who);
void remove_from_here_list(int loc, int who);
void set_where(int who, int new_loc);
int in_here_list(int loc, int who);
int somewhere_inside(int a, int b);
int in_safe_now(int who);
int subloc_here(int where, int sk);
int count_loc_structures(int where, int a, int b);

#define	city_here(a)	subloc_here((a), sub_city)

