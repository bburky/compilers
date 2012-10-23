#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "compiler.h"
#include "output.h"
#include "lexer.h"
#include "lexeme.h"

const char *input_filename;
FILE *input_file, *listing_file, *token_file, *symbol_file;
char line[74]; /* 72 chars + \n\0 */
int lineno;

/* Forward and back pointers used in lexer */
char *fptr = NULL, *bptr = NULL;

int main(int argc, const char *argv[]) {
    token tok = NONE_MATCHED;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s source_file_name\n", argv[0]);
        exit(1);
    }

    /* argv[1] is filename */
    init_lexer(argv[1]);
    init_output(argv[1]);

    while (tok.type == NONE_TYPE || tok.type != EOF_TYPE) {
        tok = get_next_token();
        write_symbol_token(lineno, tok);
        if (tok.lexeme) {
            free(tok.lexeme);
        }
    }
    return 0;
}

void init_lexer(const char *filename) {
    lineno = 0;
    input_filename = filename;
    input_file = fopen(input_filename, "r");
    if (!input_file) {
        perror(input_filename);
        exit(1);
    }
}

char* get_next_line() {
    if(!fgets(line, sizeof line, input_file)) {
        /* No more lines */
        if(feof(input_file)) {
            return NULL;
        } else {
            perror(input_filename);
            exit(1);
        }
    }
    char *newline = strstr(line, "\n"); 
    if (!newline) {
        fprintf(stderr, "Line %d is greater than 72 chars\n", lineno);
        exit(1);
    }
    lineno++;
    write_listing_line(lineno, line);
    return line;
}

token get_next_token() {
    token (*machines[])() = { misc_machine, relop_machine, longreal_machine, real_machine, int_machine };
    int current_machine;
    token matched_token;
    char *lexeme;

    /* if first line or end of current line */
    if (fptr == NULL || *fptr == '\0') {
        if (!get_next_line()) {
            return (token){ .lexeme = NULL, .type = EOF_TYPE, .attr.ptr = NULL };
        }
        fptr = line;
        bptr = line;
    }

    /* call each machine in order */
    for (current_machine = 0; current_machine < sizeof(machines)/sizeof(*machines); current_machine++) {
        matched_token = machines[current_machine]();
        if (matched_token.type != NONE_TYPE) {
            return matched_token;
        }
    }

    /* lexical error: Unrecognized symbol */
    fptr++;
    lexeme = extract_lexeme(fptr, bptr);
    bptr = fptr;
    return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = LEX_ERR_UNRECOGNIZED_SYMBOL };
}

