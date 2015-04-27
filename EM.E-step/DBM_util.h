/*
 * @(#)$Id$
 *
 *	- 
 */
#ifndef	_DBM_util_h
#define	_DBM_util_h

#ifndef		FALSE
#define		FALSE	(0)
#define		TRUE	(!FALSE)
#endif

#ifndef		WS_TOK
#define		WS_TOK  " \t\r\n"
#endif

#define		DB_HEADER_KEY  "%%dbHEADER"
#define		DB_UNSEEN_KEY  "00"

/*
 * Operation modes on DB records ...
 *
 * WARNING: These constants are interpreted by applications.
 * There is no unified semantics to such constants.
 */
#define		REC_NOP		0	/* keep unchanged (=INS) */
#define		REC_INS		1	/* insert new ignore old */
#define		REC_DEL		2	/* delete record */
#define		REC_RPL		3	/* insert new replace old */
#define		REC_UPD		4	/* insert new replace old (=RPL) */
#define		REC_APN		5	/* append data on known key */

#define		DFL_REC_DELMS	":"	/* inter-record delimiter string */
#define		DFL_FLD_DELMS	"\t"	/* intra-record field delimiter string */
#define		MAX_LEN_DELMS	80	/* max length of delimiter string +'\0' */

/* make most defaults FALSE, people may foget to initialize */
typedef struct db_ldopts {
	/* input options */
	int	txtnrm_keep_end_spaces;		/* FALSE */

	int	old_rec_overwrite_opt;		/* SKIP/RPL */

	char	rec_delms[MAX_LEN_DELMS];	/* record delimiter */
	char	fld_delms[MAX_LEN_DELMS];	/* field delimiter */

	/* output options */
	int	dat_keep_key_field;		/* keep key field as part of data */

	/* debugging options */
	int	verbose;			/* FALSE */
} DB_LDOPTS;

/*
 *
 */

#endif	/* !_DBM_util_h */
/*
 * History:
 *
 * $Log$
 */
