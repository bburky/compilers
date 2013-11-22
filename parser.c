#include "parser.h"
#include "parsergen.h"
#include "lexeme.h"
#include "compiler.h"

#include <stdlib.h>

token tok = NONE_MATCHED;

void parse() {
    while (tok.type == NONE_TYPE || tok.type != EOF_TYPE) {
        tok = get_next_token();
        parse_program();
    }
}

token match(TOKEN_TYPE type) {
    if (tok.type != type) {
        error();
    } else {
        DEBUG_PRINT(("  Matched %s\n", token_type_name[type]));
    }
    token prev_tok = tok;
    tok = get_next_token();
    return prev_tok;
}

void error() {
    puts("ERROR");
    exit(1);
}
