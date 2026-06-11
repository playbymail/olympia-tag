
#define		RANK_lord	5
#define		RANK_knight	6
#define		RANK_baron	7
#define		RANK_count	8
#define		RANK_earl	9
#define		RANK_marquess	10
#define		RANK_duke	11
#define		RANK_king	20

int garrison_here(int where);
int top_ruler(int n);
void garrison_gold(void);
char *rank_s(int who);
void touch_garrison_locs(void);
void determine_noble_ranks(void);
int may_rule_here(int who, int where);
ilist players_who_rule_here(int where);
int garrison_notices(int garr, int target);
