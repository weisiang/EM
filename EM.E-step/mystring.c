/*
 * @(#)$Id: mystring.c,v 1.2 1997/11/21 16:10:20 shin Exp shin $
 *
 *	- Functions: General string processing functions
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	<string.h>
#include	"mystring.h"

/*
 * @(#)StrStripSpace:
 *	- strip spaces within a string
 *	- return length of the new string
 *	- Note: the original string is modified in-place
 */
StrStripSpace(str)
char	*str;				/* input string */
{
	char	*s, *t;

	for(s=t=str;*s!='\0';s++)
		if (*s != ' ' && *s != '\t') { *t = *s; t++ ; }
	*t = *s;	/* '\0' */

	return(strlen(t));

} /* StrStripSpace */

/*
 * @(#)StrUpperToLower:
 *	- change all upper case letters to lower cases
 */
char	*StrUpperToLower(s)
char	*s;
{
	if (s == NULL) return(NULL);

	for (; *s != '\0' ; s++ ) {
		if ( *s >= 'A' && *s <= 'Z' )
			*s = *s - 'A' + 'a';
	}

	return(s);
} /* StrUpperToLower */

/*
 * @(#)StrDelTrailingChars:
 *
 * FUNCTION:
 *	- delete trailing characters (c) in a string (str)
 *	- str is modified in-place
 * RETURN:
 *	- new length of the string
 */
StrDelTrailingChars(str,c)
char	*str;		/* input string */
char	c;		/* trailing character to delete */
{
	char	*cp;
	int	len;

	len = strlen(str);
	if (len == 0) return(len);

	for(cp=str+len-1; cp>=str ; cp--) {	/* bug: cp!=str */
		if ( *cp != c ) return(len);
		*cp = '\0';
		len --;
	}

	return(len);

} /* StrDelAllLastChar */

/*
 * @(#)StrDelTail: replace all trailing chars in a set with NULL chars
 *	- return FALSE if the complete line is NULL after replacement
 * FUNCTION:
 *	- delete particular set of trailing characters (tchars)
 *		from the input string (istr)
 *	- istr is modified in-place
 * RETURN:
 *	- TRUE: is still a good string after deleting
 *	- FALSE: become a NULL string after deleting
 */
StrDelTail(istr, tchars)
char	*istr;		/* input string */
char	*tchars;	/* trailing chars to be deleted */
{
	char	*cp;

	cp = istr+strlen(istr)-1;
	while (cp>=istr && strchr(tchars, *cp)!= NULL)
		{ *cp = '\0'; cp --; }
		/* cp ++; *cp = '\0'; */
	if ( cp < istr )
		return(FALSE);		/* empty string */
	else
		return(TRUE);
} /* StrDelTail */

/*
 * @(#)StrDelTrailingCharClass:
 *
 * FUNCTION:
 *	- delete trailing characters (c) of a subset in a string (str)
 *	- str is modified in-place
 * RETURN:
 *	- new length of the string
 */
StrDelTrailingCharClass(str,class)
char	*str;		/* input string */
char	*class;		/* trailing character class to delete */
{
	char	*cp;
	int	len;

	len = strlen(str);
	if (len == 0) return(len);

	for(cp=str+len-1; cp>=str ; cp--) {	/* bug: cp!=str */
		if ( strchr(class, *cp) == NULL ) return(len);
		*cp = '\0';
		len --;
	}

	return(len);

} /* StrDelTrailingCharClass */

/*
 * @(#)CStrParse:
 *	Parse a C-style string which contains escape character '\'
 *	into its memory image.
 * BUG:
 *	- parsed string may exceeds tgt size
 */
CStrParse(src,tgt)
char	*src;				/* input string */
char	*tgt;				/* output string */
{
	char	*s;
	char	*t;

	t = tgt;

	for (s=src;*s!='\0';s++,t++) {
		if ( *s != '\\' )
			{*t = *s;}
		else {	s++;
			switch ( *s ) {
			case 'r': *t = '\r'; break;
			case 'n': *t = '\n'; break;
			case 't': *t = '\t'; break;
			case '\\':*t = '\\'; break;
			default:
			/* copy as is */
				*t = '\\';
				s--;
				break;	
			}
		}
	}
	*t = '\0';
} /* CStrParse */

/*
 * @(#)NumOfWords:
 *	- count number of words in a string
 *	- word delimiter is specified by (char *)delm
 */
#if 0
NumOfWords(s, delm)
char	*s;
char	*delm;
{
	char	buff[BUFSIZ];
	int	n_word;
	char	*word;

	n_word = 0;
	strcpy(buff,s);
	word = strtok(buff,delm);
	while ( word != NULL ) {
		n_word ++;
		word = strtok(NULL,delm);
	}
	return(n_word);
} /* NumOfWords */
#endif

/*
 * @(#)NumOfTokens:
 *	- count number of tokens/words in a string
 *	- word delimiter is specified by (char *)delm
 */
NumOfTokens(s,delm)
char	*s;				/* input string */
char	*delm;				/* delimiters */
{
	char		buff[BUFSIZ];
	int		n_token;
	char		*str;

	n_token = 0;
	strcpy(buff,s);
	str = strtok(buff,delm);
	while ( str != NULL ) {
		n_token ++;
		str = strtok(NULL,delm);
	}
	
	return(n_token);
} /* NumOfTokens */

/*
 * FUNCTION:
 *	Divide a string into an array of tokens.
 *
 * INPUT:
 *	char *S: input string (will not be affected)
 *	char *delimiter: a string of delimiters between tokens
 *
 * OUTPUT:
 *	int *n_token: the number of tokens in the string
 *
 * RETURN:
 *	static char *tok[]: an array of tokenized strings.
 *
 * WARNING: the output tokens are stroed in static memory,
 *	which will be overwritten on each call of this function.
 *	Therefore, the caller must copy the static memory contents
 *	to local char buffers immediately, if the tokenized strings
 *	are to be used.
 *
 * TIPS: This function could be used to count the number of tokens
 *	just like NumOfTokens() does.
 */
char	**StrGetTokens(S,delimter,n_token)
char	*S;				/* input string */
char	*delimter;			/* delimiters between tokens */
int	*n_token;			/* number of output tokens */
{
	static char	buff[BUFSIZ];	/* untokenized string */
	static char	*tok[BUFSIZ];	/* array of tokenized strings */

	char		*str;		/* current token string */

	*n_token = 0;
	strcpy(buff,S);
	str = strtok(buff,delimter);
	while ( str != NULL ) {
		tok[*n_token] = str;
		(*n_token) ++;
		str = strtok(NULL,delimter);
	}
	
	return(tok);
} /* StrGetTokens */

/*
 * @(#)StrPutTokens:
 *	- identical to StrGetTokens but with tokenized strings
 *		copied to extarnal array of string
 * FUNCTION:
 *	Divide a string into an array of tokens.
 *
 * INPUT:
 *	char *S: input string (will not be affected)
 *	char *tok[]: pointer to externally allocated array
 *	char *delimiter: a string of delimiters between tokens
 *
 * OUTPUT:
 *	char *tok[]: an array of tokenized strings.
 *	(note: the data are in static memory, which will be overwrite on
 *	each call)
 *
 * RETURN:
 *	int n_token: the number of tokens in the string
 */
StrPutTokens(S,tok,delimter)
char	*S;				/* input string */
char	*tok[];				/* array of tokenized strings */
char	*delimter;			/* delimiters between tokens */
{
	int		n_token;	/* number of output tokens */
	static char	buff[BUFSIZ];	/* untokenized string */

	char		*str;		/* current token string */

	n_token = 0;
	strcpy(buff,S);
	str = strtok(buff,delimter);
	while ( str != NULL ) {
		tok[n_token] = str;
		n_token ++;
		str = strtok(NULL,delimter);
	}
	
	return(n_token);
} /* StrPutTokens */

/*
 * @(#)StrDivide2Tokens:
 *	- identical to StrPutTokens
 *	- input string is divided in-place, instead of dividing
 *	  a static copy.
 */
StrDivide2Tokens(S,tok,delimter,alloc)
char	*S;				/* input string */
char	*tok[];				/* array of tokenized strings */
char	*delimter;			/* delimiters between tokens */
int	alloc;				/* memory allocation option */
{
	int		n_token;	/* number of output tokens */
	static char	buff[BUFSIZ];	/* untokenized string */

	char		*str;		/* current token string */

	char		*s;		/* a static/reallocated/in-place
					   copy of the input string */

	switch ( alloc ) {
	case A_STATIC:
		/* use a static copy of input string */
		strcpy(buff,S);
		s = buff;
		break;
	case A_REALLOC:
		/* allocate memory for input string */
		s = (char *)malloc(strlen(S)+1);
		strcpy(s,S);
		s[strlen(S)]='\0';
		break;
	case A_INPLACE:
	default:
		/* modify input string directly in-place */
		s = S;
		break;
	}

	n_token = 0;
	str = strtok(s,delimter);
	while ( str != NULL ) {
		tok[n_token] = str;
		n_token ++;
		str = strtok(NULL,delimter);
	}
	
	return(n_token);
} /* StrDivide2Tokens */

/*
 * @(#)StrGetKey:
 *	- Get a static copy of the key string at a specified field
 * input:
 *	str: input string where the key is to derive
 *	kid: field id for the desired key
 *		- 0: take the whole string as the key
 *	delm: field delimiters
 */
char	*StrGetKey(str,kid,delm)
char	str[];				/* input string */
int	kid;				/* field id for the desired key */
char	*delm;				/* delimiters for separating fields */
{
	static char buff[BUFSIZ];	/* local copy of the input string */

	char	*key;			/* current key */
	int	k;			/* current field of tmp key */

	strcpy(buff,str);		/* make a local copy */
	if ( kid == 0 ) {		/* the whold line */
		return(buff);
	}

	/* find the matching key (by skipping leading fields) */
	key = strtok(buff,delm); k = 1;
	while (key!=NULL && k < kid) {
		key = strtok(NULL,delm);
		k ++;
	}
	if ( key == NULL ) {
		fprintf(stderr,"Skipped: No such field %d\n", kid);
		return(NULL);
	}
	/* else ... k >= kid (==) */

	return(key);

} /* StrGetKey */

/*
 * @(#)CopyStrTokens:
 *
 * FUNCTION:
 *	- copy input tokens (itok[]) to output tokens (otok[])
 *		upto 'n_tok' tokens
 *	- the buffer area for output tokens is 'otokb', which
 *		has 'bufsiz_otokb' bytes.
 * TODO:
 *	- no preventive checks
 */
CopyStrTokens(itok, otok, n_tok, otokb, bufsiz_otokb)
char	*itok[];		/* input tokens */
char	*otok[];		/* output tokens */
int	n_tok;			/* number of tokens to copy */
char	*otokb;			/* buff for *otok[] */
int	bufsiz_otokb;		/* buffer size of otokb */
{
	int	i;

	memset(otokb, 0, bufsiz_otokb);
	for(i=0;i<n_tok;i++) {
		otok[i] = otokb;
		strcpy(otokb, itok[i]);
		otokb += strlen(itok[i]);
		*otokb = '\0';
		otokb ++;
	}

} /* CopyStrTokens */

/*
 * @(#)CatStrToTokens:
 *
 * FUNCTION:
 *	- concatenate a string to the end of the output buffer
 *		area (otokb) of output tokens (otok[]), the size
 *		of the buffer area being 'bufsiz_otokb' bytes
 *	- create a new token in array of tokens
 *	- and update the number of tokens (*n_tok) by one
 * TODO:
 *	- should check n_tok < max_n_tok
 */
CatStrToTokens(str, otok, n_tok, otokb, bufsiz_otokb)
char	*str;			/* input string */
char	*otok[];		/* output tokens */
int	*n_tok;			/* number of tokens in otok[] */
char	*otokb;			/* buff for *otok[] */
int	bufsiz_otokb;		/* buffer size of otokb */
{
	/* int	i;  */
	char	*cp;

	/* where to copy to ... */
	if ( *n_tok < 0 )
		return(FALSE);

	if ( *n_tok == 0 )
		cp = otokb;
	else
		cp = otok[*n_tok-1] + strlen(otok[*n_tok-1]) + 1;

	if ( cp + strlen(str) >= otokb + bufsiz_otokb ) {
		fprintf(stderr, "buffer length exceeded\n");
		return(FALSE);
	}

	otok[*n_tok] = cp;
	strcpy(cp, str);
/*
	cp += strlen(str);
	*cp = '\0';
*/
	(*n_tok) ++;

	return(TRUE);

} /* CopyStrTokens */

/*
 * History:
 *
 * $Log: mystring.c,v $
 * Revision 1.2  1997/11/21  16:10:20  shin
 * 1. + char *StrUpperToLower(s)
 * 2. + StrPutTokens(S,tok,delimter)
 * 3. + StrDivide2Tokens(S,tok,delimter,alloc)
 *
 * Revision 1.1  1997/09/18  07:48:14  shin
 * Initial revision
 *
 */
