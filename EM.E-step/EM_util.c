/*
 * @(#)$Id: EM_util.c , v 1.0 2010/05/05 08:26 roleno $
 *
 * - EM
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <ndbm.h>
#include <float.h>
#include "EM_util.h"
#include "DBM_util.h"
#include "mystring.h"

// func_name: load_File
// load corpus file to memory
// IN : file path
// OUT : FileText

struct FileText *load_File(char  *filename) {
  FILE  *fd;
  struct FileText *ft;
  char  *pData;
  char  buf[BLKSIZE];
  int   len = 0;
  int   totalen = 0;
  int   fileidx = 0;

  ft = (struct FileText *) malloc (sizeof(struct FileText));
  ft->n_rows = 0;
  fd = fopen (filename, "r");

//  fprintf (stdout, "File : %s open.\n", filename);

	//read line until BLKSIZE characters have been read or newline/End-of-File(EOF) is reached
  while ( fgets (buf, BLKSIZE, fd) != NULL) {
    printf("%s",buf);
    len = strlen (buf);
    totalen += len;
    ft->n_rows += 1;
  }

  fseek (fd, 0, SEEK_SET);

  ft->text = (char **) malloc (ft->n_rows * sizeof(char **) + totalen * sizeof(char *) + 1);

  pData = (char *) (ft->text+ft->n_rows);

  while ( fgets (buf, BLKSIZE, fd) != NULL) {
    ft->text[fileidx] = pData;
    len = strlen (buf);
    buf[len-1] = '\0';
    strcpy (ft->text[fileidx], buf);
    pData += len;
    ++fileidx;
  }

  fclose (fd);

  return ft;
}

// func_name : isExist
// check phrases in phrasetable
//

int isExist (struct PhraseTable *pt, char *txt, int row) {
   for (int i=0; i<row; ++i) {
     if ( 0 == strcmp (pt->phrases[i].content, txt)) {
       return 1;
     }
   }

   return 0;
}

// func_name : creatPhrase
// creat phrase uniquely. ( phrase <=>  word in chinese WS)
// IN : a sentence
// OUT : a phrase table

struct PhraseTable *creatPhrase (char *sentence) {
    struct          PhraseTable *pt;
    char            **tokens;
    static char     buf[BLKSIZE];
    int              n_token;
    int              row = 0;
    int              tmplength;
    int              diff = 0;

    // get n tokens.
    tokens = StrGetTokens (sentence, " ", &n_token);
    // malloc PhraseTable
    pt = malloc (sizeof(struct PhraseTable));
    // count phrasetablesize
    pt->n_phrase = (((n_token+1) * n_token ) / 2);
    // malloc phrase of PhraseTable
    pt->phrases = malloc (sizeof (struct Phrase) * pt->n_phrase);
    // init buffer
    strcpy (buf, "");
    // count string length.
  	for (int i=0; i<n_token; ++i) {
  	    for (int j=i; j<n_token; ++j) {
  	        diff = j-i+1;
  	        for (int k=i; k<=j; ++k) {
  	            strcat (buf, tokens[k]);
  	            strcat (buf, " ");
  	        }
  	        tmplength = strlen(buf);
  	        buf[tmplength-1] = '\0';
            if (! isExist(pt, buf, row)) {
              pt->phrases[row].content = malloc (sizeof (char) * tmplength) ;
              strcpy (pt->phrases[row].content, buf);
              pt->phrases[row].n_token = diff;
              ++row;
            } else {
              pt->n_phrase -= 1;
            }
  	        strcpy (buf, "");
  	    }
  	}

    return pt;
}

// func_name : getString
// like substring in Java or C-sharp
// IN : a token arry, start idx, end idx
// OUT: a sub-string

char *getString (char **string, int start, int end) {
    static char buf[BLKSIZE];

    strcpy (buf, "");
    for (; start <= end; ++start) {
        strcat (buf, string[start]);
        strcat (buf, " ");
    }
    int n = strlen (buf);
    buf[n-1] = '\0';

    return buf;
}

// func_name : getLogWordProb
// get a ln word prob.
// IN : a dbm, a key
// OUT : a ln word prob. (Ln-type)

double getLogWordProb (DBM *dbFile, char *key) {
    char buff[BLKSIZE];
    double word_prob = 0.0;
    int string_len = 0;

    if (db_Get_String (dbFile, key, buff)) {
#ifdef DEBUG
        printf ("Print word prob: %s\n", buff);   // for checking dbm_fetch
#endif
// if word_prob = -9.356341e-1^Q or -9.356341e1^Q
// this will replace e => '\0'
// actually, this is a termination problem
// cause 1: sprintf in EM func
// cause 2: TrainEM.sh use perl to reEstimate word_prob. i should check perl script to modify it.
// I just do some skill to avoid it.
        string_len = strlen(buff)-1;
        for (;string_len>0;string_len--) {
          if(buff[string_len] == 'e') {
             buff[string_len] = '\0';
          }
        }
#ifdef DEBUG
        printf ("Print word prob_mod: %s\n", buff);   // for checking dbm_fetch
#endif        
        word_prob = atof(buff);
    } else {
        word_prob = log(1.7e-307);
    }
fprintf(stderr ,"log_word_prob = %f\n",word_prob);
    return word_prob;
}

// func_name : getLogWordProb
// get a word prob.
// IN : a dbm, a key
// OUT : a word prob. (P-type)

double getWordProb (DBM *dbFile, char *key) {
    static char buff[BLKSIZE];
    double word_prob = 0.0;
    if (db_Get_String (dbFile, key, buff)) {
        word_prob = atof(buff);
        word_prob = exp (word_prob);
    } else {
        word_prob = 1.7e-307;
    }
fprintf(stderr ,"word_prob = %f\n",word_prob);
    return word_prob;
}

// func_name : alpha_i
// compute alpha at i
// IN : a token arr, a token arr's length, idx: i point, dbm, alpha arr
// OUT : return an alpha value at i

double alpha_i (char **tokens, int n_token, int i, DBM	*dbFile, double *alpha , int SegLen) {
    // alpha_0 = 1
    if (i == 0)
        return log(1.0);

    double ai = 0.0;

    int j = 0;
    int L = (n_token > SegLen && i > SegLen) ? SegLen : i;
    int k = 1;
    int a = 0;
    int a_i = 0;
    int a_j = 0;
    double log_word_prob = 0.0;

    for (j= i-k+1;k<=L;++k) {
        a = i-k;
        j= i-k+1;
        a_i = i-1;
        a_j = j-1;

        log_word_prob = getLogWordProb(dbFile, getString (tokens, a_j, a_i));
        
        ai += exp( log_word_prob + alpha[a] );
#ifdef DEBUG
        fprintf (stdout, "%d %.16f(%s) + %.16f(alpha[%d])\n", i, log_word_prob, getString (tokens, a_j, a_i), alpha[a], a);
#endif
    }

    return log(ai);
}

// func_name : alpha
// get 0~n alpha value
// IN : a sentence, dbm
// OUT : a alpha arr

double *alpha (char *sentence, DBM *dbFile , int SegLen) {
    double       *alpha_arr;
    char        **tokens;
    int         n_token;
    int         n;

    // get n tokens.
    tokens = StrGetTokens (sentence, " ", &n_token);
    n = n_token + 2;        // we only need n_token+1
    alpha_arr = malloc (sizeof(double) * n);

    for (int i=0; i<n; ++i)
        alpha_arr[i] = 0.0;
    for (int i=0; i<=n_token; ++i) {
        alpha_arr[i] = alpha_i (tokens, n_token, i, dbFile, alpha_arr , SegLen);
#ifdef DEBUG
        fprintf (stdout, "alpha_%d = %.16f\n", i, alpha_arr[i]);
#endif
    }

    return alpha_arr;
}

// func_name : beta_j
// compute beta at j
// IN : a token arr, a token arr's length, idx: point j, dbm, beta arr
// OUT : return a beta value at j

double beta_j (char **tokens, int n_token, int j, DBM *dbFile, double *beta , int SegLen) {
    // beta_n+1 = 1
    if (j == n_token+1)
        return log(1.0);

    double bj = 0.0;

    int i = 0;
    int diff = n_token - j;
    int L = ( diff >= SegLen ) ? SegLen : diff+1;
    int k = 1;
    int b = 0;
    int b_i = 0;
    int b_j = 0;
    double log_word_prob = 0.0;

    for (i= j+k-1;k<=L;++k) {
        b = j+k;
        i= j+k-1;
        b_i = i-1;
        b_j = j-1;

        log_word_prob = getLogWordProb(dbFile, getString (tokens, b_j, b_i));

        bj += exp (log_word_prob + beta[b]);
#ifdef DEBUG
        fprintf (stdout, "%d %.16f(%s) + %.16f(beta[%d])\n", (j-1), log_word_prob, getString (tokens, b_j, b_i), beta[b], (b-1));
#endif    
    }

    return log(bj);
}

// func_name : beta
// get 1~n+1 beta
// IN : a sentence,  dbm
// OUT: a beta arr

double *beta (char *sentence, DBM *dbFile , int SegLen) {
    double       *beta_arr;
    char        **tokens;
    int         n_token;
    int         n;

    // get n tokens.
    tokens = StrGetTokens (sentence, " ", &n_token);
    n = n_token + 2;
    beta_arr = malloc (sizeof(double) * n);

    for (int i=0; i<n; ++i)
        beta_arr[i] = 0.0;
    for (int j=n-1; j>=1; --j) {
        beta_arr[j] = beta_j (tokens, n_token, j, dbFile, beta_arr , SegLen);
#ifdef DEBUG
        fprintf (stdout, "beta_%d = %.16f\n", (j-1), beta_arr[j]);
#endif    
    }

    return beta_arr;
}

// func_name : getWordExpectCount
// get an expected count of a word in a sentence
// IN : a sentence, a word, dbm, alpha arr, beta arr
// OUT : an expected count of a word.

double getWordExpectCount (char *sentence, char *word, DBM *dbWP, double *alpha, double *beta) {
    double except_count = 0.0;
    int n_token = 0;
    int len = 0;
    int c_i = 0;
    int c_j = 0;
    char **tokens;
    char tok[BLKSIZE];
    double log_word_prob = 0.0;
    double current_count = 0.0;

    tokens = StrGetTokens (sentence, " ", &n_token);

    for (int i=0; i<n_token; i++) {
        for (int j=i; j<n_token; j++) {
            strcpy (tok, "");

            for (int k=i; k<=j; k++) {
                strcat (tok, tokens[k]);
  	            strcat (tok, " ");
            }

            len = strlen (tok);
            tok[len-1] = '\0';

            if ( 0 == strcmp (tok, word) ) {
              current_count = 0.0;
              c_i = i;
              c_j = j+2;

              log_word_prob = getLogWordProb(dbWP, word);

              current_count = exp ( alpha[c_i] + log_word_prob + beta[c_j] - alpha[n_token] );
              except_count += current_count;
#ifdef DEBUG                
              fprintf (stdout, "(%f\t%s) = %.16f + %.16f + %.16f - %.16f\n", except_count, word, alpha[c_i], getLogWordProb(dbWP, word), beta[c_j], alpha[n_token]);
              fprintf (stdout, "Current (%f\t%s)\n", current_count, word);
#endif
            }
        }

    }

    return except_count;
}


// main_func : EM
// IN : file, db_word_prob(for computing), db_count(for storing)
// OUT : no type

void EM (char *filename, char *db_name, char *db_count_name, int SegLen) {
    DBM	*db_word_prob;
    DBM *db_expected_count;
    struct FileText *ft = load_File (filename);
    struct PhraseTable *pt;
    struct ForwardBackward *fb;
    char buf[BLKSIZE];
    char *sentence;
    double expect_count;
    double current_count, total_count = 0.0;
    int n_rows = ft->n_rows;
    int is_old;

// open word_prob and expected_count dbm.
// load corpus into memory
//
    db_Open_ReadWrite (db_name, &db_word_prob);
    db_Open_ReadWrite (db_count_name, &db_expected_count);
    fb = malloc (sizeof(struct ForwardBackward));

// start to compute expected count
//
//

    for (int sent=0; sent<n_rows; ++sent) {
        sentence = ft->text[sent];
        fprintf (stdout, "Sentence: %s\n", sentence);
        fb->alpha = alpha (sentence, db_word_prob , SegLen);       // get alpha
        fb->beta = beta (sentence, db_word_prob , SegLen);         // get beta

        // phrase <=> word in this program.
        // creat phrases uniqueliy
        pt = creatPhrase (sentence);

        // scan 
        for (int p = 0; p < pt->n_phrase; p++) {
            if (pt->phrases[p].n_token <= SegLen && db_Is_Old_Record (db_word_prob, pt->phrases[p].content)) {
                // return P type
                current_count = getWordExpectCount (sentence, pt->phrases[p].content, db_word_prob, fb->alpha, fb->beta);
                if (db_Is_Old_Record (db_expected_count, pt->phrases[p].content))
		{
                    // return P type
                    expect_count = getWordProb (db_expected_count, pt->phrases[p].content);
                    expect_count += current_count;

		    // if expect_count => inf, given a small value
                    if ( 1.7e-307 > expect_count)
		    {
                      expect_count = 1.7e-307;
                    }

                    sprintf (buf, "%f", log(expect_count)); 
                    db_Update_String (db_expected_count, pt->phrases[p].content, buf, &is_old);
#ifdef DEBUG
                    fprintf (stdout, "Update: %f %s, %s\n", expect_count, buf, pt->phrases[p].content);
#endif
                }
		else
		{
                    expect_count = current_count;
                    
		    // if expect_count => inf, given a small value
                    if ( 1.7e-307 > expect_count)
		    {
                      expect_count = 1.7e-307;
                    }
                    
		    sprintf (buf, "%f", log(expect_count));
                    db_Update_String (db_expected_count, pt->phrases[p].content, buf, &is_old);
#ifdef DEBUG
                    fprintf (stdout, "New: %f %s, %s\n", expect_count, buf, pt->phrases[p].content);
#endif
                }
                total_count += current_count;
            }
        }
    }

    db_Close (db_word_prob);
    db_Close (db_expected_count);
}
