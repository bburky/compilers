#ifndef PARSER_H
#define PARSER_H

#include "lexeme.h"

token tok;

void parse();

int match(TOKEN_TYPE);

void error();

void synch(TOKEN_TYPE[], int);

#endif