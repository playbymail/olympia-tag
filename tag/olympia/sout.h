/* BUGFIX (modernization): include forward declarations for varargs */
#include "legacy.h"

/*
 *  Possible desinations of output:
 */

#define	VECT	(-1)		/* vector of recipients */
				/* n >= 0: output to entity event log */
#define	MASTER	(-2)

#define	OUT_SUMMARY	0
#define	OUT_BANNER	0
#define	OUT_INCLUDE	1
#define	OUT_LORE	2
#define	OUT_NEW		3	/* new player listing */
#define	OUT_LOC		4	/* location descriptions */
#define	OUT_TEMPLATE	5	/* order template */
#define	OUT_GARR	6	/* garrison log */
#define	OUT_SHOW_POSTS	7	/* show what press and rumor look like */

/*
 *  tags for log_output()
 */

#define	LOG_CODE	10	/* Code alerts */
#define	LOG_SPECIAL	11	/* Special events */
#define LOG_DEATH	12	/* Character deaths */
#define	LOG_MISC	13	/* Other junk */
#define	LOG_DROP	14	/* Player drops */

/*
 *  tags for eat.c
 */

#define	EAT_ERR		20	/* Errors in orders submitted */
#define	EAT_WARN	21	/* Warnings in orders submitted */
#define	EAT_QUEUE	22	/* Current order queues */
#define	EAT_HEADERS	23	/* Email headers bounced back */
#define	EAT_OKAY	24	/* Regular (non-error) output for scanner */
#define	EAT_PLAYERS	25	/* Player list */


extern ilist out_vector;


extern int out_path;			/* alternate sout directive */
extern int out_alt_who;			/* used if path == MASTER */

extern char *spaces;
extern int spaces_len;

