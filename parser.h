#ifndef PARSER_H
#define PARSER_H

#include "lexeme.h"

token tok;

void parse();

token match(TOKEN_TYPE);

void error();

#endif