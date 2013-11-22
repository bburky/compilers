#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "compiler.h"
#include "output.h"
#include "parser.h"
#include "lexer.h"
#include "lexeme.h"

const char *input_filename;
char line[74]; /* 72 chars + \n\0 */
int lineno;

/* Forward and back pointers used in lexer */
char *fptr = NULL, *bptr = NULL;

/*
 * main function
 * Intitialize lexer and reserved word list, run main loop
 */

int main(int argc, const char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s source_file_name reserved_word_file_name\n", argv[0]);
        exit(1);
    }

    /* argv[1] is filename, argv[2] is reserved_word_file */
    init_lexer(argv[1]);
    init_reserved_words(argv[2]);
    init_output(argv[1]);

    DEBUG_PRINT(("PARSING\n"));
    parse();
    return 0;
}

/*
 * Initialize lexer
 * Open the source file
 */
void init_lexer(const char *filename) {
    lineno = 0;
    input_filename = filename;
    input_file = fopen(input_filename, "r");
    if (!input_file) {
        perror(input_filename);
        exit(1);
    }
}

/*
 * Initialize reserved word list
 * Read all the reserved words into a list of tokens.
 * Also intialize the symbol list.
 */
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

/*
 * Read the next line from the source file
 * Return char* to the line.
 */
char* get_next_line() {
    size_t len;
    if(!fgets(line, sizeof line, input_file)) {
        /* No more lines */
        if(feof(input_file)) {
            return NULL;
        } else {
            perror(input_filename);
            exit(1);
        }
    }

    /* clean up string */
    char *newline = strnstr(line, "\n", LEX_MAX_LINE+2);
    if (!newline) {
        len = strnlen(line, LEX_MAX_LINE+2);
        if (len > LEX_MAX_LINE) {
            fprintf(stderr, "Line %d is greater than 72 chars\n", lineno);
            exit(1);
        }
        line[len] = '\n';
        line[len+1] = '\0';
    }
    lineno++;
    write_listing_line(lineno, line);
    return line;
}

/*
 * Read the next token and match it to all machines
 * Run each machine in sequence and return a lexical error if unmatched
 */
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
        if (matched_token.type == WHITESPACE_TYPE) {
            /* Don't even return whitespace tokens */
            return get_next_token();
        } else if (matched_token.type != NONE_TYPE) {
            if (tok.type == LEXERR_TYPE) {
                write_listing_lexerr(lineno, tok);
            }
            write_token(lineno, tok);
            return matched_token;
        }
    }

    /* lexical error: Unrecognized symbol */
    fptr++;
    lexeme = extract_lexeme(fptr, bptr);
    bptr = fptr;
    write_listing_lexerr(lineno, tok);
    return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = LEX_ERR_UNRECOGNIZED_SYMBOL };
}

