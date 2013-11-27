#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "lexeme.h"

token tok;

int parse();

int match(TOKEN_TYPE);

void error();

void synch(TOKEN_TYPE[], int);

typedef enum {
    NONE, INTEGER, REAL, BOOLEAN, PROCEDURE, ERROR, ERROR_STAR
} type;

// link points backwards in a stack for variables and procedures
// link node points forwards in a linked list for parameters
typedef struct stack_node {
    const char *id;
    const type id_type;
    struct stack_node *parameters;
    struct stack_node *link;
} stack_node;


stack_node* get_procedure();

stack_node* check_parameter(stack_node *procedure, char *id);

stack_node* check_add_parameter(char *id, type id_type);

stack_node* pop_procedure();

stack_node* check_id(char* id, bool scope);

stack_node* check_add_id(char* id, type id_type);

#endif