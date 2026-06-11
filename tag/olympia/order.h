
char *top_order(int player, int who);
void pop_order(int player, int who);
void queue_order(int player, int who, char *s);
void prepend_order(int pl, int who, char *s);
void queue_stop(int pl, int who);
void load_orders(void);
void save_orders(void);

void delete_box(int n);
void change_box_kind(int n, int kind);
void alloc_box(int n, int kind, int sk);
int new_ent(int kind, int sk);
void list_pending_orders_sup(int who, int num, int show_empty);
int stop_order(int pl, int who);
