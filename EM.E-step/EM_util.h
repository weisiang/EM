/*
 * @(#)$Id: EM_util.h , v 1.0 2010/05/05 08:26 roleno $
 *
 * - EM
 */



#ifndef _EM_util_h
#define _EM_util_h

#define BLKSIZE 1024
#define SEGMENT_LENGTH 4

struct FileText {
    char	**text;
    int		n_rows;
};

struct Phrase {
    char	*content;
    int		n_token;
    double	word_prob;
};

struct PhraseTable {
	struct Phrase	*phrases;
	int		n_phrase;
};

struct ForwardBackward {
    double *alpha;
    double *beta;
};

void EM(char *filename, char *db_name, char *db_count_name, int SegLen);

#endif
