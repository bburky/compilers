#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.h"
#include "output.h"

#define DEBUG

#define DEBUG_PRINT(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define DEBUG_TOKEN(fptr,bptr,str) printf("\n%s\nLexeme: %.*s\n", str, (int)(fptr-bptr), bptr)

/*
#define DEBUG_TOKEN(fptr,bptr,str) \
            do { if (DEBUG) fprintf(stderr, "\n%s\nLexeme:%.*s\n", str, (int)(fptr-bptr), bptr); } while (0)
*/

const char *input_filename;

FILE *input_file, *token_file;

char *fptr, *bptr;

void init_lexer(const char*);

char* get_next_line(void);

token get_next_token(void);

#endif
