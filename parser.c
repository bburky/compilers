#include "parser.h"
#include "parsergen.h"
#include "lexeme.h"
#include "compiler.h"

token tok = NONE_MATCHED;

void parse() {
    while (tok.type == NONE_TYPE || tok.type != EOF_TYPE) {
        tok = get_next_token();
        parse_program();
    }
}

int match(TOKEN_TYPE type) {
    if (tok.type != type) {
        DEBUG_PRINT(("  FAILED TO MATCH %s\n", token_type_name[type]));
        error();
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

void error() {
    puts("SYNTAX ERROR");
}
