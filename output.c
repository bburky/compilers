#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "output.h"
#include "lexeme.h"

/*
 * Initialize output listing file and token file
 * Open the files for writing and write the header of the token file.
 */
void init_output(const char *filename) {
    size_t len = strlen(filename);
    char *tmpfilename;

    tmpfilename = malloc(len+5);
    if (!tmpfilename) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    
    strcpy(tmpfilename, filename);
    strcat(tmpfilename, ".lst");
    listing_file = fopen(tmpfilename, "w");
    if (!listing_file) {
        perror(tmpfilename);
        exit(1);
    }

    strcpy(tmpfilename, filename);
    strcat(tmpfilename, ".tok");
    token_file = fopen(tmpfilename, "w");
    if (!token_file) {
        perror(tmpfilename);
        exit(1);
    }
    
    strcpy(tmpfilename, filename);
    strcat(tmpfilename, ".add");
    address_file = fopen(tmpfilename, "w");
    if (!address_file) {
        perror(tmpfilename);
        exit(1);
    }
    
    free(tmpfilename);
    fprintf(token_file, "%-10s%-20s%-20s%s\n", "Line No.", "Lexeme", "TOKEN-TYPE", "ATTRIBUTE");
}

/*
 * Write a source line to the listing file
 */
void write_listing_line(int lineno, const char *line) {
    /* line is guaranteed to contain a newline */
    fprintf(listing_file, "%-10d%s", lineno, line);
}

/*
 * Write all lexical errors with the token to the listing file
 */
void write_listing_lexerr(int lineno, token tok) {
    int i;

    for (i = 0; i < NUM_LEX_ERR_TYPES; i++) {
        if (tok.attr.errtype & (1 << i)) {
            fprintf(listing_file, "%-10s%-40s%s\n", "LEXERR:", lex_err_name[i][1], tok.lexeme);
            fflush(listing_file);
        }
    }
}

/*
 * Write all syntax errors with the token to the listing file
 */
void write_listing_synerr(int lineno, token tok, char* nonterminal, TOKEN_TYPE expected[], int expected_len) {
    char expected_str[100] = { [0] = '\0' };
    if (expected_len > 1) {
        strncat(expected_str, "one of ", 100);
    }
    for (int i = 0; i < expected_len; i++) {
        if (i > 0) {
            strncat(expected_str, ", ", 100);
        }
        strncat(expected_str, token_type_name[expected[i]], 100);
    }

    if (tok.type == EOF_TYPE) {
        fprintf(listing_file, "SYNERR:   Failed to parse %s, at end of file, expected %s\n", nonterminal, expected_str);
    } else {
        fprintf(listing_file, "SYNERR:   Failed to parse %s, got token \"%s\", expected %s\n", nonterminal, tok.lexeme, expected_str);
    }
    fflush(listing_file);
}

/*
 * Write all symantic errors to the listing file
 */
//void write_listing_symerr(char* message) {
//    fprintf(listing_file, "SYMERR:   %s\n", message);
//}
// see macro

/*
 * Write a token to the token file
 * All lexical errors in a token are listed
 */
void write_token(int lineno, token tok) {
    int i;
    char attrstr[100] = { [0] = '\0' };
    
    /* line is guaranteed to contain a newline */
    switch (tok.type) {
        case LEXERR_TYPE:
            for (i = 0; i < NUM_LEX_ERR_TYPES; i++) {
                if (tok.attr.errtype & (1 << i)) {
                    if (*attrstr != '\0') {
                        strncat(attrstr, ", ", 100);
                    }
                    strncat(attrstr, lex_err_name[i][0], 100);
                }
            }
            fprintf(token_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.errtype, attrstr);
            break;

        case ID_TYPE:
            fprintf(token_file, "%-10d%-20s%-2d %-17s%p\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.ptr);
            break;

        case NUM_TYPE:
            fprintf(token_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.num, num_attr_name[tok.attr.num]);
            break;

        case RELOP_TYPE:
            fprintf(token_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.relop, relop_attr_name[tok.attr.relop]);
            break;

        case MULOP_TYPE:
            fprintf(token_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.mulop, mulop_attr_name[tok.attr.mulop]);
            break;

        case ADDOP_TYPE:
            fprintf(token_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.addop, addop_attr_name[tok.attr.addop]);
            break;

        case WHITESPACE_TYPE:
            /* Don't output symbol */
            break;
            
        case EOF_TYPE:
            /* Don't output lexeme */
            fprintf(token_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, "", tok.type, token_type_name[tok.type], tok.attr.addop, "NULL");
            break;

        default:
            fprintf(token_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], 0, "NULL");
            break;
            
    }
    
}

void write_address_procedure(const char* procedure) {
    fprintf(address_file, "\nProcedure %s:\n", procedure);
}

void write_address_variable(const char* variable, int addr) {
    fprintf(address_file, "%s\t%d\n", variable, addr);
}
