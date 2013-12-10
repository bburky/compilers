#ifndef OUTPUT_H
#define OUTPUT_H

#include "lexeme.h"

FILE *listing_file, *token_file, *address_file;

void init_output(const char *filename);

void write_listing_line(int lineno, const char *line);

void write_listing_lexerr(int lineno, token tok);

void write_token(int lineno, token tok);

void write_listing_synerr(int lineno, token tok, char* nonterminal, TOKEN_TYPE expected[], int expected_len);

//void write_listing_symerr(char* message);
#define write_listing_symerr(format, ...) fprintf(listing_file, "SYMERR:   " format "\n", __VA_ARGS__)

void write_address_procedure(const char* procedure);

void write_address_variable(const char* variable, int addr);

#endif
