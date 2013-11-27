#ifndef PARSER_H
#define PARSER_H

#include "lexeme.h"

token tok;

int parse();

int match(TOKEN_TYPE);

void error();

void synch(TOKEN_TYPE[], int);

typedef enum {
    NONE, INTEGER, REAL, BOOLEAN, PROCEDURE, ERROR, ERROR_STAR
} type;

typedef struct stack_node {
    const char *id;
    const type id_type;
    struct stack_node *parameters;
    struct stack_node *prev;
} stack_node;


#endif