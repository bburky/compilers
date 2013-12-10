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

stack_node *current_procedure = NULL;

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
        .link = NULL,
        .parent = current_procedure
    };

    if (procedure == prev_param) {
        // No parameters in parameter list
        procedure->parameters = param;
    } else {
        prev_param->link = param;
    }

    return param;
}


stack_node* pop_procedure(stack_node* procedure) {
    stack_node* old_stack = stack;
    
    current_procedure = procedure->parent;
    stack = procedure;

    return old_stack;
}

stack_node* check_id(const char* id, bool scope) {
    bool above_current_procedure = false;
    
    for (stack_node* cur_stack = stack; cur_stack != NULL; cur_stack = cur_stack->link) {
        if (cur_stack->id == id) {
            return cur_stack;
        } else if (cur_stack->id_type == PROCEDURE) {
            if (current_procedure == cur_stack) {
                // Don't examine parameters until we are at or above the current procedure
                above_current_procedure = true;
            }
            
            if (above_current_procedure) {
                // Check parameters if it matches the procedure we are interested in
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
        .link = stack,
        .parent = current_procedure
    };
    stack = var;
    
    if (id_type == PROCEDURE) {
        current_procedure = var;
    }
    
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