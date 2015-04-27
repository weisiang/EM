/*
 * @(#)$Id$
 *
 *	- Functions: 
 */
#include	<stdio.h>

#if		!(defined(WIN32)||defined(USE_BDBM))
#include	<ndbm.h>	/* for unix & clones */
#else		/* WIN32||USE_BDBM */
#define		DB_DBM_HSEARCH  1
			/* enable historical interfaces in db.h with this flag */
#include	<db.h>
			/* use the new header file */
#endif		/* !(WIN32||USE_BDBM) */

#include	<fcntl.h>
#include	"mystring.h"
#include	"DBM_macro.h"
#include	"DBM_util.h"

/*
 * @(#)db_Open_ReadWrite: Open/Create an old/new DB for read/write
 *
 * FUNCTION:
 *	- open/create a database of a given name for read/writing
 *	- create if does not exists
 * INPUT:
 *	- DbName: name of database
 * OUTPUT:
 *	- dbFile: (pointer to) Opened DB file
 * USAGE:
 *	char DbName[]; DBM* MyDb;
 *	status = db_Open_ReadWrite(DbName, &MyDB)
 * RETURN:
 *	- TRUE: successful open
 *	- FALSE: fail to open
 */
db_Open_ReadWrite(DbName, dbFile)
char	*DbName;
DBM	**dbFile;
{

	DBM	*db;

	if ( (db = dbm_open(DbName,(O_CREAT|O_RDWR),0644))==(DBM *)NULL) {
		fprintf(stderr,"Fail to open New database %s\n", DbName);
		return(FALSE);
	}

	*dbFile = db;
	return(TRUE);

} /* db_Open_ReadWrite */

/*
 * @(#)db_Open_ReadOnly: Open an old DB for reading (only)
 *
 * FUNCTION:
 *	- open an OLD database of a given name for reading
 * INPUT:
 *	- DbName: name of database
 * OUTPUT:
 *	- dbFile: (pointer to) Opened DB file
 * USAGE:
 *	char DbName[]; DBM* MyDb;
 *	status = db_Open_ReadOnly(DbName, &MyDb);
 * RETURN:
 *	- TRUE: successful open
 *	- FALSE: fail to open
 */
db_Open_ReadOnly(DbName, dbFile)
char	*DbName;
DBM	**dbFile;
{

	DBM     *db;

	if ( (db = dbm_open(DbName, O_RDONLY, 0444)) == (DBM *) NULL ) {
		fprintf(stderr, "Fail to open old database %s\n", DbName);
		return(FALSE);
	}

	*dbFile = db;
	return(TRUE);

} /* db_Open_ReadOnly */

/*
 * @(#)db_Close: close an opened DB
 *
 * FUNCTION:
 *	- close an opened DB
 *	*** It's important to close a DB after opening ***
 * INPUT:
 *	- dbFile: opened DB file pointer
 * OUTPUT:
 *	- n/a
 * RETURN:
 *	- TRUE:
 */
db_Close(dbFile)
DBM	*dbFile;
{

	dbm_close(dbFile);
	return(TRUE);

} /* db_Close */

/*
 * @(#)db_Insert_String: insert a <key,string> pair into DB without replacement
 *
 * FUNCTION:
 *	- insert a <key, string> pair into a DB w/o replacement
 *		- will not replace if the key already exists
 *		- will generate an error status instead
 *		- 1st record will stay unchanged
 * INPUT:
 *	- Key: key for the data entry
 *	- String: string as the data for the entry
 * OUTPUT:
 *	- *is_old: original key exists
 * RETURN:
 *	- TRUE: successful insertion
 *	- FALSE: fail to insert
 */
db_Insert_String(dbFile, Key, String, is_old)
DBM	*dbFile;
char	*Key;			/* searching key string */
char	*String;		/* data string */
int	*is_old;		/* old record exists (NULL: don't return) */
{
	datum	key, content;
	int	status;

	/* create key & content */
	db_Use_String_As_Key(key, Key);
	db_Use_String_As_Value(content, String);

	if ( (status = dbm_store(dbFile, key, content, DBM_INSERT)) < 0 ) {
        	fprintf(stderr, "db_Insert_String: cannot insert data\n");
        	return(FALSE);
	}

#ifdef	DBX_DB_INS
        fprintf(stderr, "status= %d key= %s\n", status, key);
#endif	/* DBX_DB_INS */

	if ( is_old != NULL )
		*is_old = ( status == 1 ? TRUE : FALSE);

	return(TRUE);

} /* db_Insert_String */

/*
 * @(#)db_Replace_String: insert a <key, string> pair into DB with replacement
 *
 * FUNCTION:
 *	- insert a <key, string> pair into a DB w/ replacement
 * INPUT:
 *	- Key: key for the data entry
 *	- String: string as the data for the entry
 * OUTPUT:
 *	- *is_old: original key exists
 * RETURN:
 *	- TRUE: successful replacement
 *	- FALSE: fail to replace
 */
db_Replace_String(dbFile, Key, String, is_old)
DBM	*dbFile;
char	*Key;			/* searching key string */
char	*String;		/* data string */
int	*is_old;		/* old record exists (NULL: don't return) */
{
	datum	key, content;
	int	status;

	/* create key & content */
	db_Use_String_As_Key(key, Key);
	db_Use_String_As_Value(content,String);

	if ( (status = dbm_store(dbFile, key, content, DBM_REPLACE)) < 0 ) {
        	fprintf(stderr, "db_Replace_String: cannot replace data\n");
        	return(FALSE);
	}

#ifdef	DBX_DB_INS
        fprintf(stderr, "status= %d key= %s\n", status, key);
#endif	/* DBX_DB_INS */

	if ( is_old != NULL )
		*is_old = ( status == 1 ? TRUE : FALSE);

	return(TRUE);

} /* db_Replace_String */

/*
 * @(#)db_Update_String: insert/replace a <key,string> pair in DB
 *
 * FUNCTION:
 *	- insert/replace a <key,string> pair into a DB
 *	- virtually the same as db_Replace_String()
 *	- MAYBE THIS FUNCTION CAN BE REMOVED
 * INPUT:
 *	- Key: key for the data entry
 *	- String: new string to be updated
 * OUTPUT:
 *	- *is_old: original key exists
 * RETURN:
 *	- TRUE: successful update
 *	- FALSE: fail to update
 */
db_Update_String(dbFile, Key, String, is_old)
DBM	*dbFile;
char	*Key;			/* searching key string */
char	*String;		/* data string */
int	*is_old;		/* old record exists (NULL: don't return) */
{
	int	status;

	if ( db_Is_Old_Record(dbFile, Key) ) {	/* replace old record */
		status = db_Replace_String(dbFile, Key, String, is_old);
	} else {				/* add new record */
		status = db_Insert_String(dbFile, Key, String, is_old);
	}
	return(status);

} /* db_Update_String */

/*
 * @(#)db_Append_String: insert (if new)/append (if old) a <key,string> pair into DB
 *
 * FUNCTION:
 *	- insert a <key,string> pair into a DB if key is new
 *	- append <string> to the end of old content if key is old
 *	- a record delimiter (which may be different from field delimiter)
 *		can be specified to identify the record boundary
 *		if old and new strings do not belong to the same item
 * INPUT:
 *	- Key: key for the data entry
 *	- String: new string to be insert/appended
 *	- Rec_Delms: record delimiter string (NULL: use default)
 * OUTPUT:
 *	- *is_old: original key exists
 * RETURN:
 *	- TRUE: successful append
 *	- FALSE: fail to append
 * WAS:
 *	- db_Update_String()
 */
db_Append_String(dbFile, Key, String, Rec_Delms, is_old)
DBM	*dbFile;
char	*Key;			/* searching key string */
char	*String;		/* data string */
char	*Rec_Delms;		/* record delimiter string (NULL: use default) */
int	*is_old;		/* old record exists (NULL: don't return) */
{
	int	status;
	char	OldString[BUFSIZ];
	/* WARN: we assume that the BUFSIZ is large enough */
	int	len;		/* accumulative length to OldString */
	int	real_length;	/* real sum of lengths if appended */

	/* if exists append to old record */
	if ( db_Get_String(dbFile, Key, OldString) ) {	/* exists */
		real_length = len = strlen(OldString);

		if ( Rec_Delms != NULL ) {
			real_length += strlen(Rec_Delms);
			strncat(OldString, Rec_Delms, BUFSIZ-1-len);
			len = strlen(OldString);
		} else {	/* use default record delimiter string */
			real_length += strlen(DFL_REC_DELMS);
			strncat(OldString, DFL_REC_DELMS, BUFSIZ-1-len);
			len = strlen(OldString);
		}
		if ( String != NULL ) {
			real_length += strlen(String);
			strncat(OldString, String, BUFSIZ-1-len);
			len = strlen(OldString);
		}
		if ( len < real_length ) {
			fprintf(stderr,
			"WARN: db_Append_String overflow (key= %s) (truncated to %d bytes)\n", Key, BUFSIZ-1);
		}

		status = db_Replace_String(dbFile, Key, OldString, is_old);
	} else {				/* add new record */
		status = db_Insert_String(dbFile, Key, String, is_old);
	}
	return(status);

} /* db_Append_String */

/*
 * @(#)db_Delete_Record: delete the record with a given Key from DB
 *
 * FUNCTION:
 *	- delete the record with a given Key from DB
 * INPUT:
 *	- dbFile: DB pointer
 *	- Key: key string
 * OUTPUT:
 *	- N/A
 * RETURN:
 *	- TRUE: the specified record IS deleted successfully
 *	- FALSE: the specified record IS NOT deleted successfully
 */
db_Delete_Record(dbFile, Key)
DBM	*dbFile;
char	*Key;			/* searching key string */
{
	datum	key;
	int	status;

	/* create key */
	db_Use_String_As_Key(key, Key);

	/* delete record */
	if ( (status = dbm_delete(dbFile, key)) < 0 ) {	/* -1 */
        	fprintf(stderr, "db_Delete_Record: cannot delete record\n");
        	return(FALSE);
	}

	return ( status == 0 ? TRUE : FALSE);	/* successful deletion */

} /* db_Delete_Record */

/*
 * @(#)db_Get_String: get the String of a <Key, String> pair in DB by Key
 *
 * FUNCTION:
 *	- get the String of a <Key, String> pair in DB by Key
 *	- and return TRUE/FALSE for exisitence/non-existence of that Key
 * INPUT:
 *	- dbFile: DB pointer
 *	- Key: key string
 *	- String: buffer pointer for the returned string
 *		- or NULL if want to ignore the returned string
 * OUTPUT:
 *	- String: the returned string or NULL (if Key does not exists)
 * RETURN:
 *	- TRUE: the specified record exists
 *	- FALSE: the specified record does not exist
 */
db_Get_String(dbFile, Key, String)
DBM	*dbFile;
char	*Key;
char	*String;
{
	datum key;
	datum content;

	/* create key */
	db_Use_String_As_Key(key, Key);

	/* try to fetch from DB */
	content = dbm_fetch(dbFile, key);

	if ( content.dptr != NULL ) {	/* record exists */
		if (String != NULL)
			strcpy(String, content.dptr);
			/* WARNING: assume .dptr zero terminated */
		/* bcopy(content.dptr, (char*) &Struct, content.dsize); */
		/* bcopy(content.dptr, (char*) String, content.dsize); */
		return(TRUE);
	}
	else {				/* record not found */
		if (String != NULL) *String = '\0';
		/* if (String != NULL) String = NULL; */ /* fix: 2005/5/15 */

		return(FALSE);
	}

} /* db_Get_String */

/*
 * @(#)db_Is_Old_Record: check if the record with a given Key exists in DB
 *
 * FUNCTION:
 *	- check if the record with a given Key exists in DB
 * INPUT:
 *	- dbFile: DB pointer
 *	- Key: key string
 * OUTPUT:
 *	- N/A
 * CALL:
 * 	- db_Get_String()
 * RETURN:
 *	- TRUE: the specified record exists
 *	- FALSE: the specified record does not exist
 */
db_Is_Old_Record(dbFile, Key)
DBM	*dbFile;
char	*Key;
{
	return(db_Get_String(dbFile, Key, NULL));

} /* db_Is_Old_Record */

/* TODO: DBM_convert.c ... */

/*
 * @(#)db_Set_Default_LoadOpts:
 *
 *	- make most defaults FALSE, people may forget to initialize ...
 */
db_Set_Default_LoadOpts(opt)
DB_LDOPTS	*opt;
{
	/* input options */
	opt->txtnrm_keep_end_spaces = FALSE;
	opt->old_rec_overwrite_opt = REC_NOP;

	strcpy(opt->rec_delms, DFL_REC_DELMS);
	strcpy(opt->fld_delms, DFL_FLD_DELMS);

	/* output options */
	opt->dat_keep_key_field = FALSE;

	/* debugging options */
	opt->verbose = FALSE;

} /* db_Set_Default_LoadOpts */

/*
 * @(#)db_String_to_Key_Data_Pair: create <key,data> pair from a String
 *
 * FUNCTION:
 *	- create <key,data> pair from a String
 * INPUT:
 *	- Str: input string
 *	- i_delms: delimiter characters in input string
 *	- kid: field ID to be used as key [0..n_token-1]
 *	- key_as_data: whether to keep key as part of data or not
 *	- o_delms: output delimiter string
 *	- Key: pointer to Key buffer
 *	- Data: pointer to Data buffer
 * OUTPUT:
 *	- Key: produced Key
 *	- Data: produced data
 * RETURN:
 *	- n: number of tokens used as data
 */
db_String_to_Key_Data_Pair(Str, i_delms, kid, key_as_data, o_delms, Key, Data)
char	*Str;		/* input string */
char	*i_delms;	/* delimiter characters in input string */
int	kid;		/* field ID to be used as key [0..n_token-1] */
int	key_as_data;	/* whether to keep key as part of data or not */
char	*o_delms;	/* output delimiter string */
char	*Key;		/* produced Key */
char	*Data;		/* produced data */
{
	char	**toks;		/* tokenized Str */
	int	n_token;	/* number of tokens in Str */
	int	i;		/* token index */
	int	n;		/* number of tokens in Data */

	/* divide into tokens */
	toks = StrGetTokens(Str, i_delms, &n_token);

	/* create Key */
	if (Key != NULL) strcpy(Key, toks[kid]);

	/* init & create Data */
	strcpy(Data, "");
	for( i=0, n=0; i < n_token; i++) {
		/* skip key, if not explicitly asked to preserve */
		if ( i==kid && !key_as_data ) continue;

		/* concat. current token with new delimiters */
		if (n!=0) strcat(Data, o_delms);
		strcat(Data, toks[i]); n++;
	} /* i */

	return(n);

} /* db_String_to_Key_Data_Pair */

/*
 * @(#)db_Load_From_Text:
 *
 * FUNCTION: convert Table in text file to DBM format.
 *	- new DB will be created and old DB will be destroyed
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * RETURN:
 *	- TRUE:
 *	- FALSE:
 */
db_Load_From_Text(fin, ldopts, kid, o_delms, db)
FILE		*fin;			/* input file */
DB_LDOPTS	*ldopts;		/* loading options */
int		kid;			/* fileld index of keys (0, 1, 2, ...) */
char		*o_delms;		/* output field delimiters */
DBM		*db;			/* data base for the inputs */
{
	char		dbHeader[BUFSIZ];	/* db info <attr=val:...> */

	datum		key, content;		/* DBM key-content pair */

	int		status;			/* return status */
	int		n_dup;
	int		n_rec;
	char		ibuff[BUFSIZ];
	char		obuff[BUFSIZ];
	char		**tok;
	int		n_token;
	int		i;
	int		is_old;
	int		n_data_fields;		/* number of fields in record */
	static char	*default_delms = DFL_FLD_DELMS /* "\t" -2005/5/15-shin- */;
	char		*delms;

	/* determine output field delimiter */
	delms = ( o_delms != NULL ? o_delms : default_delms);

        /* Get lines and process */
	for ( n_dup = n_rec = 0 ;
		fgets(ibuff, BUFSIZ, fin) != NULL; n_rec++) {

		if ( ! ldopts->txtnrm_keep_end_spaces )
			StrDelTrailingCharClass(ibuff, WS_TOK);

		tok = StrGetTokens(ibuff, WS_TOK, &n_token);

		if ( n_token == 0 ) {	/* ignore empty lines */
			continue;
		}

		/* save to DB with key[kid] as the searching key */
		if ( kid >= n_token ) {
			fprintf(stderr,
			"ERR: db_Load_From_Text: short record at: %d: %s\n",
				n_rec + 1, ibuff);
			return(FALSE);
		}

		/* create buffer for non-key fields */
		*obuff = '\0';
		n_data_fields = 0;
		for(i=0 ; i<n_token; i++)
			if ( i != kid ) {
				if ( n_data_fields != 0 )
					strcat(obuff, delms);
				strcat(obuff, tok[i]);
				n_data_fields ++;
			}

		if ( ldopts->verbose ) {
			fprintf(stderr,"key= %s\ndat= %s\n",
				tok[kid], obuff);
		}

		switch ( ldopts->old_rec_overwrite_opt ) {
			case REC_APN:	/* Append/Merge */
				db_Append_String(db, tok[kid], obuff, ldopts->rec_delms, &is_old);
				break;
			case REC_RPL:
			case REC_UPD:	/* same semantic as RPL */
				db_Replace_String(db, tok[kid], obuff, &is_old);
				break;
			case REC_NOP:	/* default as insert */
			case REC_INS:
			default:
				db_Insert_String(db, tok[kid], obuff, &is_old);
				break;
		}

		if ( is_old ) { n_dup++;
			fprintf(stderr,"WARN: dup key: %s rec_no %d\n",
				tok[kid], n_rec + 1);
		}

	} /* fgets */

	/* save header infomation to DB with DB_HDRKEY as the searching key */

	sprintf(dbHeader, "%s=%d%s%s=%d",
		"Nrec", n_rec, delms, "Nuniq", n_rec-n_dup);

	db_Insert_String(db, DB_HEADER_KEY, dbHeader, &is_old);

	return(TRUE);

} /* db_Load_From_Text */

/*
 * @(#)db_Save_As_Text: save DB entries to a text file sequentially
 *
 * FUNCTION:
 *	- save DB entries to a text file sequentially
 * INPUT:
 *
 * OUTPUT:
 *
 * RETURN:
 *	- TRUE:
 *	- FALSE:
 */
db_Save_As_Text(db, delms, OutFile)
DBM	*db;		/* DB file to be saved */
char	*delms;		/* delimiter string between <key,value> */
FILE	*OutFile;	/* output text file */
{

	datum   key, content;

	for (key=dbm_firstkey(db); key.dptr != NULL; key=dbm_nextkey(db)) {

		content = dbm_fetch(db, key);

		db_Print_Datum(OutFile, NULL,  key, NULL);

		db_Print_Datum(OutFile, delms, content, "\n");
	}

	return(TRUE);

} /* db_Save_As_Text */

/*
 * @(#)db_Lookup_Text:
 *
 * FUNCTION:
 *	- lookup words/keys in a word list file
 * INPUT:
 *	- fi: input file
 *		- each line as a key or
 *		- each token in each line as a key
 *	- dbDic: DB or dictionary to lookup
 *	- i_delms: delimiting characters that delimit keys in an input line
 *		- if NULL, each line is regarded as a single key
 *	- fo: output file to receive lookup results
 * OUTPUT:
 *	- *fo: <key, data> (separated by TAB) for each line
 * RETURN:
 *	- TRUE:
 *	- FALSE:
 * REVISION:
 *	- 2005/06/03
 * AUTHOR:
 *	- Jing-Shin Chang
 */
db_Lookup_Text(dbDic, fi, i_delms, fo)
DBM	*dbDic;		/* db/dictionaty */
FILE	*fi;		/* key/word list file */
char	*i_delms;	/* delimiters (lookup by word)/NULL (by line) */
FILE	*fo;		/* output of dictionary data */
{
	char	ibuff[BUFSIZ];
	char	Definition[BUFSIZ];

	char	**tok;
	int	n_tok;
	int	i_tok;

        /* get lines and process */
        while ( fgets(ibuff, BUFSIZ, fi) != NULL ) {

		/* clean up CR/NL/SP */
		StrNromalizeCRNL(ibuff);

		if ( i_delms == NULL ) {	/* the whole line as a key */
			n_tok = 1;
			tok[0] = ibuff;
		}
		else {			/* each token is a key */
			tok = StrGetTokens(ibuff, i_delms /* WS_TOK */, &n_tok);
		}

		for (i_tok=0; i_tok< n_tok; i_tok++) {

			if ( db_Get_String(dbDic, tok[i_tok], Definition) ) {
				fprintf(fo, "%s\t%s\n", tok[i_tok], Definition);
			}
			else {
				fprintf(fo, "%s\tUNKNOWN_KEY\n", tok[i_tok]);
			}
		}

	} /* fgets */

	return(TRUE);

} /* db_Lookup_Text */

/*
 * @(#)db_Print_Datum: print chars in a datum to full length or till a '\0'
 *
 * FUNCTION:
 *	- print characters in a datum to full length or till a '\0'
 * INPUT:
 *
 * OUTPUT:
 *
 * RETURN:
 *	- TRUE:
 *	- FALSE:
 * NOTE:
 *	- the datum are not '\0' terminated, because it can be
 *              binary or ASCII, and thus are not handled as strings
 */
db_Print_Datum(OutFile, leading, data, ending)
FILE	*OutFile;		/* output file */
char    *leading;		/* leading mark (or NULL) */
datum   data;			/* datum to be printed */
char	*ending;		/* ending mark (or NULL) */
{
        int     i;

	/* dump leading mark, if available */
	if ( leading != NULL ) fprintf(OutFile, "%s", leading);

#ifdef	DBX_PRINT_DATUM
        fprintf(OutFile, "size= %4d value= ", data.dsize);
#endif	/* DBX_PRINT_DATUM */

	/* print data to full length or till a '\0' */
        for (i=0; i < data.dsize && data.dptr[i]!= '\0' ; i++) {
                fprintf(OutFile, "%c", data.dptr[i]);
        }

        if ( ending != NULL ) fprintf(OutFile, "%s", ending);
	/* was: fprintf(OutFile, "\n"); */

	return(TRUE);

} /* db_Print_Datum */

/*
 * History:
 *
 * $Log$
 */
