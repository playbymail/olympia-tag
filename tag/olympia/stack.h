
int here_preceeds(int a, int b);
int stack_parent(int who);
int stack_leader(int who);
int stacked_beneath(int a, int b);
void leave_stack(int who);
void join_stack(int who, int stack);
void drop_stack(int who, int to_drop);
void promote_stack(int lower, int higher);
void take_prisoner(int who, int target);
int has_prisoner(int who, int pris);
int give_prisoner(int who, int target, int pris);
int move_prisoner(int who, int target, int pris);
void promote(int who, int new_pos);
int check_prisoner_escape(int who, int chance);
void prisoner_escapes(int who);
int stack_contains_priest(int who);
int contains_mu_undead(int who);
int stack_contains_mu_undead(int who);
