

struct skill_ent *p_skill_ent(int who, int skill);
struct skill_ent *rp_skill_ent(int who, int skill);
int has_skill(int who, int skill);
int has_subskill(int who, int subskill);
void set_skill(int who, int skill, int know);

int skill_school(int sk);
void list_skills(int who, int num, char *prefix);

int learn_skill(int who, int sk);
char *exp_s(int level);
int forget_skill(int who, int skill);

#define NOT_TAUGHT 0
#define TAUGHT_SPECIFIC -1
#define TAUGHT_GENERIC -2
#define TAUGHT_STUDYPOINTS -3

