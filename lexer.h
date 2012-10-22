#ifndef LEXER_H
#define LEXER_H

#include "lexeme.h"

token whitespace_machine(void);

token relop_machine(void);

token longreal_machine(void);

token real_machine(void);

token int_machine(void);

token mulop_machine(void);

token addop_machine(void);

#endif
