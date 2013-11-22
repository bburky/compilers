#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include "lexeme.h"

/* #define DEBUG */

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

const char *input_filename;

FILE *input_file;

char *fptr, *bptr;

void init_lexer(const char*);

void init_reserved_words(const char*);

char* get_next_line(void);

token get_next_token(void);

#endif
