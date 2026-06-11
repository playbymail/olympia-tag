/* BUGFIX (modernization): use an updated malloc/realloc/free */
#include "../../lib/checked_alloc.h"
/* BUGFIX (modernization): update lists to use 64-bit pointers */
#include "../../lib/lists.h"
/* BUGFIX (modernization): forward declarations are required for varadic functions */
#include "legacy.h"

#define	SYSV

#define	TRUE	1
#define	FALSE	0

#define		LEN		2048	/* generic string max length */

#define	abs(n)		((n) < 0 ? ((n) * -1) : (n))

#define	isalpha(c)	(((c)>='a' && (c)<='z') || ((c)>='A' && (c)<='Z'))
#define	isdigit(c)	((c) >= '0' && (c) <= '9')
#define	iswhite(c)	((c) == ' ' || (c) == '\t')

#if 1
#define	tolower(c)	(lower_array[c])
extern char lower_array[];
#else
#define	tolower(c)	(((c) >= 'A' && (c) <= 'Z') ? ((c) - 'A' + 'a') : (c))
#endif

#define	toupper(c)	(((c) >= 'a' && (c) <= 'z') ? ((c) - 'a' + 'A') : (c))

#if 0
#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) < (b) ? (a) : (b))
#endif

// char *str_save(char *);
//
// char *getlin(FILE *);
// char *getlin_ew(FILE *);
// int i_strncmp(char *s, char *t, int n);
// int i_strcmp(char *s, char *t);
// int fuzzy_strcmp(char *, char *);
// int rnd(int low, int high);
//
// int readfile(char *path);
// char *readlin(void);
// char *readlin_ew(void);
// char *eat_leading_trailing_whitespace(char *s);
