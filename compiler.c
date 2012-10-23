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

    if (argc != 3) {
        fprintf(stderr, "Usage: %s source_file_name reserved_word_file_name\n", argv[0]);
        exit(1);
    }

    /* argv[1] is filename, argv[2] is reserved_word_file */
    init_lexer(argv[1]);
    init_reserved_words(argv[2]);
    init_output(argv[1]);

    while (tok.type == NONE_TYPE || tok.type != EOF_TYPE) {
        tok = get_next_token();
        if (tok.type == LEXERR_TYPE) {
            write_listing_lexerr(lineno, tok);
        }
        write_token(lineno, tok);
        if (tok.lexeme) {
//            free(tok.lexeme);
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

void init_reserved_words(const char *filename) {
    char *reserved_word;
    int token_type, token_attr;
    FILE *file;
    token *tok;
    token_node *node;
    token_node *prev_node = NULL;

    symbol_list = NULL;

    file = fopen(filename, "r");
    if (!file) {
        perror(filename);
        exit(1);
    }
    
    reserved_word = malloc(LEX_MAX_ID+1);
    if (!reserved_word) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }

    while (fscanf(file, "%10s %d %d", reserved_word, &token_type, &token_attr) != -1) {
        tok = malloc(sizeof(token));
        if (!tok) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }

        node = malloc(sizeof(token_node));
        if (!node) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }

        tok->lexeme = reserved_word;
        tok->type = (TOKEN_TYPE)token_type;
        tok->attr.addop = (ADDOP_ATTR)token_attr;

        if (prev_node) {
            prev_node->node = node;
        } else {
            reserved_word_list = node;
        }
        node->tok = tok;
        prev_node = node;

        reserved_word = malloc(LEX_MAX_ID+1);
        if (!reserved_word) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }
    }

    node->node = NULL;
    free(reserved_word);
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
    token (*machines[])() = { misc_machine, idres_machine, relop_machine, longreal_machine, real_machine, int_machine };
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

