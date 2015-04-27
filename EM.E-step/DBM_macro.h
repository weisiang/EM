/*
 * @(#)$Id$
 *
 *	- Macro Expansion for frequently used statements
 */
#ifndef	_DBM_macro_h
#define	_DBM_macro_h

/*
 *	Macro Expansion for frequently used statements
 */

/*
 * @(#)db_Use_String_As_Key: create key using string format
 */
#define	db_Use_String_As_Key(key, KeyStr)		\
{							\
	/* create key */				\
	key.dptr = KeyStr;				\
	key.dsize = strlen(key.dptr);			\
}

/*
 * @(#)db_Use_String_As_Value: encode record content using string format
 */
#define	db_Use_String_As_Value(content, String)		\
{							\
/*							\
 * use '\0' terminated string as record content		\
 */							\
	content.dptr = String;				\
	content.dsize = strlen(content.dptr) + 1;	\
	/* with ending '\0' */				\
}


#endif	/* !_DBM_macro_h */
/*
 * History:
 *
 * $Log$
 */
