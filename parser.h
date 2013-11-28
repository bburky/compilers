#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "lexeme.h"

typedef enum {
    NONE, ARRAY_INTEGER, ARRAY_REAL, INTEGER, REAL, BOOLEAN, PROCEDURE, ERROR, ERROR_STAR
} type;

// link points backwards in a stack for variables and procedures
// link node points forwards in a linked list for parameters
typedef struct stack_node {
    const char *id;
    type id_type;
    struct stack_node *parameters;
    struct stack_node *link;
} stack_node;

token tok;

// TODO: don't export this in header file
stack_node *stack;

bool parse();

bool match(TOKEN_TYPE);

void error();

void synch(TOKEN_TYPE[], int);

stack_node* get_procedure();

stack_node* check_parameter(stack_node *procedure, const char *id);

stack_node* check_add_parameter(stack_node *prev_param, const char *id);

stack_node* pop_procedure();

stack_node* check_id(const char* id, bool scope);

stack_node* check_add_id(const char* id, type id_type, bool scope);

stack_node* set_parameter_types(stack_node* params, type param_type);

stack_node* pop_children(stack_node* node);

#endif