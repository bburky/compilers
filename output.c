#include <stdio.h>

#include "compiler.h"

void init_output(const char *filename) {
    /* TODO: do real file output */
    listing_file = stdout;
    symbol_file = stdout;
    fprintf(symbol_file, "%-10s%-15s%-20s%s\n", "Line No.", "Lexeme", "TOKEN-TYPE", "ATTRIBUTE");
}

void write_listing_line(int lineno, const char *line) {
    /* line is guaranteed to contain a newline */
    fprintf(listing_file, "%-10d%s", lineno, line);
}

void write_symbol_token(int lineno, token tok) {    
    /* line is guaranteed to contain a newline */
    switch (tok.type) {
        case LEXERR_TYPE:
            fprintf(symbol_file, "%-10d%-15s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.relop, "...");
            break;

        case RELOP_TYPE:
            fprintf(symbol_file, "%-10d%-15s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.relop, relop_attr_name[tok.attr.relop]);
            break;

        case MULOP_TYPE:
            fprintf(symbol_file, "%-10d%-15s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.mulop, mulop_attr_name[tok.attr.mulop]);
            break;

        case ADDOP_TYPE:
            fprintf(symbol_file, "%-10d%-15s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.addop, addop_attr_name[tok.attr.addop]);
            break;

        /* Don't output some lexeme strings */
        case WHITESPACE_TYPE:
        case EOF_TYPE:
            fprintf(symbol_file, "%-10d%-15s%-2d %-17s%-2d %s\n", lineno, "", tok.type, token_type_name[tok.type], tok.attr.addop, "NULL");
            break;

        default:
            fprintf(symbol_file, "%-10d%-15s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], 0, "NULL");
            break;
            
    }
    
}
