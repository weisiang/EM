/*
 * @(#)$Id: mystring.h,v 1.2 1997/11/18 03:23:21 shin Exp shin $
 *
 *	- General string processing macros/utilities
 */
#ifndef	_mystring_h
#define	_mystring_h
#include	<string.h>		/* strchr() */

#ifndef		FALSE
#define		FALSE	(0)
#define		TRUE	(!FALSE)
#endif

/*
 * memory allocation options for some tokenization functions
 */
#define		A_INPLACE	0	/* modify original copy directly */
#define		A_STATIC	1	/* use a static copy */
#define		A_REALLOC	2	/* re-allocate and make a new copy */

/*
 * General string processing macros ...
 */
#define		SkipChar(cp,c)		\
		while ( *cp != '\0' && *cp == c ) { cp ++; }
#define		SkipCharClass(cp,class)		\
		while ( *cp != '\0' && strchr(class,*cp)!=NULL ) { cp ++; }

#define		StrDelLastChar(str,c)	\
		if(str[strlen(str)-1]==(c))str[strlen(str)-1]='\0'

#define		StrNromalizeCRNL(str)	\
		{\
			StrDelLastChar((str),'\n');	 	\
			StrDelLastChar((str),'\r');		\
			StrDelTrailingChars((str), ' ');	\
		}

/*
 * increment char pointer until it points to a non-special char
 *	(e.g., non-white space character) or end of string
 */
#define	NextNonSpcChar(cp,ws)	{while(*cp!='\0'&&index(ws,*cp)!=NULL)cp++;}
#define	NextSpcChr(cp,ws)	{while(*cp!='\0'&&index(ws,*cp)==NULL)cp++;}

#define	NextNonSpace(cp,ws)	NextNonSpcChar(cp,ws)
#define	NextSpace(cp,ws)	NextSpcChr(cp,ws)

/*
 * move char pointer to the end of a token or end of string
 */
#define	GetEndOfToken(start,delm,end)	\
	{end=start;while(*end!='\0'&&index(delm,*end)==NULL)end++;end--;}

/*
 * Renamed modules (for upward compatibility only ...)
 */

#define	strip_space(str)	StrStripSpace(str)

extern	char	*StrUpperToLower();
extern	char	**StrGetTokens();
extern	char	*StrGetKey();

#endif	/* !_mystring_h */

/*
 * History:
 *
 * $Log: mystring.h,v $
 * Revision 1.2  1997/11/18  03:23:21  shin
 * 1. + support the following memory allocation options for some
 *    + tokenization functions:
 *    +    A_INPLACE, A_STATIC, A_REALLOC
 * 2. + NextNonSpcChar(cp,ws)
 * 3. + NextSpcChr(cp,ws)
 * 4. + NextNonSpace(cp,ws)
 * 5. + NextSpace(cp,ws)
 * 6. + GetEndOfToken(start,delm,end)
 * 7. + extern char *StrUpperToLower();
 *
 * Revision 1.1  1997/09/18  07:51:35  shin
 * Initial revision
 *
 */
