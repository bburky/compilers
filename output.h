#ifndef OUTPUT_H
#define OUTPUT_H

#include "lexeme.h"

FILE *listing_file, *symbol_file;

void init_output(const char *filename);

void write_listing_line(int lineno, const char *line);

void write_listing_lexerr(int lineno, token tok);

void write_token(int lineno, token tok);

#endif
