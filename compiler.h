#ifndef COMPILER_H
#define COMPILER_H

#include "lexeme.h"

const char *input_filename;

FILE *input_file;

char *fptr, *bptr;

void init_lexer(const char*);

void init_reserved_words(const char*);

char* get_next_line(void);

token get_next_token(void);

#endif
