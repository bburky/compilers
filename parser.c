#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "parser.h"
#include "parsergen.h"
#include "lexeme.h"
#include "compiler.h"
#include "output.h"

token tok = NONE_MATCHED;

int parse() {
    tok = get_next_token();
    type program_type = parse_program();
    if (program_type == ERROR || program_type == ERROR_STAR)
        return 1;
    return 0;
}

int match(TOKEN_TYPE type) {
    if (tok.type != type) {
        DEBUG_PRINT(("  FAILED TO MATCH %s\n", token_type_name[type]));
        write_listing_synerr(lineno, tok, token_type_name[type], (TOKEN_TYPE[]) {type}, 1);
        return 0;
    } else {
        DEBUG_PRINT(("  Matched %s\n", token_type_name[type]));
    }
    tok = get_next_token();
    return 1;
}

int token_type_in_list(TOKEN_TYPE token, TOKEN_TYPE list[], int len) {
    for (int i = 0; i < len; i++) {
        if (list[i] == token) {
            return 1;
        }
    }
    return 0;
}

void synch(TOKEN_TYPE synch_set[], int len) {
    while (!token_type_in_list(tok.type, synch_set, len)) {
        DEBUG_PRINT(("  SYNCH: DISCARING TOKEN %s\n", token_type_name[tok.type]));
        tok = get_next_token();
    }
    DEBUG_PRINT(("  SYNCH: SYNCHED WITH TOKEN %s\n", token_type_name[tok.type]));
}

stack_node *stack = NULL;

stack_node* check_procedure(char *id) {
    for (stack_node* cur_stack = stack; cur_stack != NULL; cur_stack = cur_stack->link) {
        if (cur_stack->id == id) {
            return cur_stack;
        }
    }
    // Didn't find id in stack
    return NULL;
}

//stack_node* check_add_procedure(char *id) {
//    
//}


stack_node* get_procedure() {
    for (stack_node* cur_stack = stack; cur_stack != NULL; cur_stack = cur_stack->link) {
        if (cur_stack->id_type == PROCEDURE) {
            return cur_stack;
        }
    }
    // Should not be reached: should always have at least one procedure on stack
    assert(false);
}

stack_node* check_parameter(stack_node *procedure, char *id) {
    for (stack_node* cur_param = procedure->parameters; cur_param != NULL; cur_param = cur_param->link) {
        if (cur_param->id == id) {
            return cur_param;
        }
    }
    // Didn't find id in parameters
    return NULL;
}


stack_node* check_add_parameter(char *id, type id_type) {
    // stack_node *procedure = get_procedure();
    // procedure will be on top of stack
    stack_node *procedure = stack;

    if (!check_parameter(procedure, id)) {
        // Parameter already exists
        return NULL;
    }
    
    stack_node *param = malloc(sizeof(stack_node));
    if (!param) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    *param = (stack_node) {
        .id = id,
        .id_type = id_type,
        .parameters = NULL,
        .link = NULL
    };

    if (procedure->parameters == NULL) {
        // No parameters in parameter list
        procedure->parameters = param;
    } else {
        // Find end of parameter list and append the new one
        stack_node* cur_param;
        for (cur_param = procedure->parameters; cur_param->link != NULL; cur_param = cur_param->link);
        cur_param->parameters = param;
    }
    

    return param;
}


stack_node* pop_procedure() {
    // Don't pop empty stack
    if (!stack) {
        return NULL;
    }
    // Find the first procedure on the stack, and set the stack to point before it
    for (stack_node *cur_stack = stack->link, *cur_stack_next = stack; cur_stack_next != NULL; cur_stack_next = cur_stack, cur_stack = cur_stack->link) {
        if (cur_stack_next->id_type == PROCEDURE) {
            stack = cur_stack;
            return cur_stack_next;
        }
    }
    // Should not be reached: stack was not empty and did not contain a procedure
    assert(false);
}

stack_node* check_id(char* id, bool scope) {
    for (stack_node* cur_stack = stack; cur_stack != NULL; cur_stack = cur_stack->link) {
        if (cur_stack->id == id) {
            return cur_stack;
        } else if (cur_stack->id_type == PROCEDURE) {
            // Reached end of current scope
            for (stack_node* cur_param = cur_stack->parameters; cur_param != NULL; cur_param = cur_param->link) {
                if (cur_param->id == id) {
                    return cur_param;
                }
            }
            // Didn't find id in parameters: if searching current scope only, stop
            if (scope) {
                return NULL;
            }
        }
    }
    // Reached top stack
    return NULL;
}

stack_node* check_add_id(char* id, type id_type) {
    if (!check_id(id, true)) {
        // id already exists
        return NULL;
    }
    
    stack_node *var = malloc(sizeof(stack_node));
    if (!var) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    *var = (stack_node) {
        .id = id,
        .id_type = id_type,
        .parameters = NULL,
        .link = stack
    };
    stack = var;
    return stack;
}
