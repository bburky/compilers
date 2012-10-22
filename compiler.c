#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "compiler.h"

const char *input_filename;
FILE *input_file, *listing_file, *token_file, *symbol_file;
char line[74]; /* 72 chars + \n\0 */

/* Forward and back pointers used in lexer */
char *fptr = NULL, *bptr = NULL;

int main(int argc, const char *argv[]) {
    token *tok = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s source_file_name\n", argv[0]);
        exit(1);
    }

    /* argv[1] is filename */
    init_lexer(argv[1]);


    while (tok == NULL || tok->type != EOF_TYPE) {
        tok = get_next_token();
        /* do nothing */
        free(tok);
    }
    return 0;
}

void init_lexer(const char *filename) {
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
        fprintf(stderr, "Line greater than 72 chars\n");
        fprintf(stderr, "Line is %d chars long: %s\n", (int) strlen(line), line);
        exit(1);
    }
    return line;
}

token* get_next_token() {
    token *(*machines[])() = {whitespace_machine, addop_machine, relop_machine, mulop_machine, longreal_machine, real_machine, int_machine};
    /* TODO: token *(*current_machine)() = *machines; */
    int current_machine;
    token *matched_token;

    /* if first line or end of current line */
    if (fptr == NULL || *fptr == '\0') {
        if (!get_next_line()) {
            DEBUG_TOKEN(NULL, NULL, "EOF");
            matched_token = malloc(sizeof(token));
            if (!matched_token) {
                fprintf(stderr, "Out of memory");
                exit(1);
            }
            matched_token->lexeme = NULL;
            matched_token->type = EOF_TYPE;
            matched_token->attr.ptr = NULL;
            return matched_token;
        }
        fptr = line;
        bptr = line;
    }

    /* call each machine in order */
    for (current_machine = 0; current_machine < 7; current_machine++) {
        matched_token = machines[current_machine]();
        if(matched_token) {
            return matched_token;
        }
    }

    /* lexical error: Unrecognized symbol */
    fptr++;
    DEBUG_TOKEN(fptr, bptr, "LEXERR: Unrecognized symbol");
    matched_token = malloc(sizeof(token));
    if (!matched_token) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    matched_token->lexeme = NULL;
    matched_token->type = LEXERR_TYPE;
    matched_token->attr.errtype = LEX_ERR_UNRECOGNIZED_SYMBOL;
    bptr = fptr;
    return matched_token;
}

