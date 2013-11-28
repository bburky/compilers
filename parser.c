#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "parser.h"
#include "parsergen.h"
#include "lexeme.h"
#include "compiler.h"
#include "output.h"

token tok = NONE_MATCHED;

stack_node *stack = NULL;

bool parse() {
    tok = get_next_token();
    type program_type = parse_program();
    if (program_type == ERROR || program_type == ERROR_STAR)
        return true;
    return false;
}

bool match(TOKEN_TYPE type) {
    if (tok.type != type) {
        DEBUG_PRINT(("  FAILED TO MATCH %s\n", token_type_name[type]));
        write_listing_synerr(lineno, tok, token_type_name[type], (TOKEN_TYPE[]) {type}, 1);
        return false;
    } else {
        DEBUG_PRINT(("  Matched %s\n", token_type_name[type]));
    }
    tok = get_next_token();
    return true;
}

bool token_type_in_list(TOKEN_TYPE token, TOKEN_TYPE list[], int len) {
    for (int i = 0; i < len; i++) {
        if (list[i] == token) {
            return true;
        }
    }
    return false;
}

void synch(TOKEN_TYPE synch_set[], int len) {
    while (!token_type_in_list(tok.type, synch_set, len)) {
        DEBUG_PRINT(("  SYNCH: DISCARING TOKEN %s\n", token_type_name[tok.type]));
        tok = get_next_token();
    }
    DEBUG_PRINT(("  SYNCH: SYNCHED WITH TOKEN %s\n", token_type_name[tok.type]));
}

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

stack_node* check_parameter(stack_node *procedure, const char *id) {
    for (stack_node* cur_param = procedure->parameters; cur_param != NULL; cur_param = cur_param->link) {
        if (cur_param->id == id) {
            return cur_param;
        }
    }
    // Didn't find id in parameters
    return NULL;
}


stack_node* check_add_parameter(stack_node *prev_param, const char *id) {
    // stack_node *procedure = get_procedure();
    // procedure will be on top of stack
    stack_node *procedure = stack;
    assert(procedure->id_type == PROCEDURE);

    // Don't check for NULL dummy id
    if (id && check_parameter(procedure, id)) {
        write_listing_symerr("Redeclaration of parameter \"%s\"", id);
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
        .id_type = NONE,
        .parameters = NULL,
        .link = NULL
    };

    if (procedure == prev_param) {
        // No parameters in parameter list
        procedure->parameters = param;
    } else {
        prev_param->link = param;
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

stack_node* check_id(const char* id, bool scope) {
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

stack_node* check_add_id(const char* id, type id_type, bool scope) {
    if (check_id(id, scope)) {
        write_listing_symerr("Redeclaration of identifier \"%s\"", id);
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

stack_node* set_parameter_types(stack_node* params, type param_type) {
    stack_node *prev_param = NULL;
    for (stack_node *cur_param = params; cur_param != NULL; prev_param = cur_param, cur_param = cur_param->link) {
        // Should only be setting the type of params that are currently none
        assert(cur_param->id_type == NONE);
        cur_param->id_type = param_type;
    }

    return prev_param;
}

stack_node* pop_children(stack_node* node) {
    // TODO: probably do address calculation at this point
    stack_node *children = node->link;
    node->link = NULL;
    stack = node;
    return children;
}

