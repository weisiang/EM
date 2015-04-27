/*
 * @(#)$Id: EM.c , v 1.0 2010/05/05 08:26 roleno $
 *
 * - EM
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <ndbm.h>
#include "mystring.h"
#include "EM_util.h"

void help() {
  fprintf (stdout, "Usage: EM [-h] [-f file] [-d db] [-c db_count] [-l SegLen].\n");
} 

int main (int argc, char **argv)
{
    int ch;
    int fileflag = 0;
    int dbflag = 0;
    int db_countflag = 0;
    int SegLenFlag = 0;//flin insert
    char *filename = NULL;
    char *db_name = NULL;
    char *db_count_name = NULL;
    int SegLen = 0;//flin insert
    char *opts = "f:d:c:l:h";

    while( -1 != (ch = getopt (argc, argv, opts) ) ) {
        switch(ch) {
            case 'f':
                fileflag = 1;
                filename = optarg;
                break;
            case 'd':
                dbflag = 1;
                db_name = optarg;
                break;
            case 'c':
                db_countflag = 1;
                db_count_name = optarg;
                break;
	    case 'l'://flin insert
		SegLenFlag = 1;
		SegLen = atoi(optarg);
		break;
            case 'h':
                help();
                return 0;
            break;
            case '?':
                fprintf (stderr, "Error option.\n");
                return 1;
            default:
                fprintf (stderr, "Some exceptions.\n");
                return 1;
        }
    }

    if (!fileflag)  {
      fprintf (stderr, "No input file.\n\n");
      help();
      return 0;
    }
    
    if (!dbflag)  {
      fprintf (stderr, "No db file.\n\n");
      help();
      return 0;
    }

    if (!db_countflag) {
      fprintf (stderr, "No db_count file.\n\n");
      help();
      return 0;
    }

    //flin insert
    if(!SegLenFlag)
    {
      fprintf(stderr , "No Segment Length.\n\n");
      help();
      return 0;
    }

    // main function.
    EM (filename, db_name, db_count_name , SegLen);
    
    return 0;
}
