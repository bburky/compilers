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

void check_add_variable(token, type);

void check_add_procedure(token);

void check_add_parameter(token, type);

void check_variable(token, type);

void check_procedure(token);

void check_parameter(token, type);

void pop_procedure();


