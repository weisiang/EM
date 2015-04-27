#
#	make the EM algorithm
#
BINDIR			= ../bin
CC			= gcc
CPPC			= g++
CFLAGS			= -O3

all:
	${CC} EM.c EM_util.c EM_util.h mystring.h mystring.c DBM_macro.h DBM_util.c DBM_util.h -o EM -std=c99 -Wall -DDEBUG -D_GNU_SOURCE ${CFLAGS} -lgdbm_compat -lm 
