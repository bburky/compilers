#ifndef COMPILER_H
#define COMPILER_H

#include "lexer.h"
#include "output.h"

const char *input_filename;

FILE *input_file, *token_file;

char *fptr, *bptr;

void init_lexer(const char*);

char* get_next_line(void);

token get_next_token(void);

#endif
