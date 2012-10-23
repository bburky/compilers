#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "output.h"
#include "lexeme.h"

void init_output(const char *filename) {
    /* TODO: do real file output */
    listing_file = stdout;
    symbol_file = stdout;
    fprintf(symbol_file, "%-10s%-20s%-20s%s\n", "Line No.", "Lexeme", "TOKEN-TYPE", "ATTRIBUTE");
}

void write_listing_line(int lineno, const char *line) {
    /* line is guaranteed to contain a newline */
    fprintf(listing_file, "%-10d%s", lineno, line);
}

void write_symbol_token(int lineno, token tok) {
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
                    strncat(attrstr, lex_err_name[i], 100);
                }
            }
            fprintf(symbol_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.errtype, attrstr);
            break;

        case ID_TYPE:
            fprintf(symbol_file, "%-10d%-20s%-2d %-17s%p\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.ptr);
            break;

        case NUM_TYPE:
            fprintf(symbol_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.num, num_attr_name[tok.attr.num]);
            break;

        case RELOP_TYPE:
            fprintf(symbol_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.relop, relop_attr_name[tok.attr.relop]);
            break;

        case MULOP_TYPE:
            fprintf(symbol_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.mulop, mulop_attr_name[tok.attr.mulop]);
            break;

        case ADDOP_TYPE:
            fprintf(symbol_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], tok.attr.addop, addop_attr_name[tok.attr.addop]);
            break;

        case WHITESPACE_TYPE:
            /* Don't output symbol */
            break;
            
        case EOF_TYPE:
            /* Don't output lexeme */
            fprintf(symbol_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, "", tok.type, token_type_name[tok.type], tok.attr.addop, "NULL");
            break;

        default:
            fprintf(symbol_file, "%-10d%-20s%-2d %-17s%-2d %s\n", lineno, tok.lexeme, tok.type, token_type_name[tok.type], 0, "NULL");
            break;
            
    }
    
}
