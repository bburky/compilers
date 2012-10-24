#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "compiler.h"
#include "lexer.h"
#include "lexeme.h"

/* generate arrays of strings for types and symbols using x-macros */
#define X(a) #a,
char *token_type_name[] = {
    TOKEN_TYPE_TABLE
};
char *relop_attr_name[] = {
    RELOP_ATTR_TABLE
};
char *mulop_attr_name[] = {
    MULOP_ATTR_TABLE
};
char *addop_attr_name[] = {
    ADDOP_ATTR_TABLE
};
char *num_attr_name[] = {
    NUM_ATTR_TABLE
};
char *lex_err_name[][2] = {
    {"INT_TOO_LONG", "Integer too long"},
    {"REALINT_TOO_LONG", "Integer portion too long"},
    {"FRAC_TOO_LONG", "Fractional portion too long"},
    {"EXP_TOO_LONG", "Exponent portion too long"},
    {"LEADING_ZERO", "Leading zero"},
    {"ID_TOO_LONG", "Identifier too long"},
    {"UNRECOGNIZED_SYMBOL", "Unrecognized symbol"}
};
#undef X

extern inline char* extract_lexeme(const char *fptr, const char *bptr);

extern inline char* make_lexeme(const char *str);

/*
 * Check if symbol already exists, if not add it.
 * Return pointer to symbol.
 */
symbol_node* add_symbol(const char *lexeme) {
    symbol_node *prevsym, *sym, *newsym;

    for (prevsym = NULL, sym = symbol_list; sym != NULL ; prevsym = sym, sym = sym->sym) {
        if (!strcmp(sym->id, lexeme)) {
            return sym;
        }
    }
    newsym = malloc(sizeof(symbol_node));
    if (!newsym) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    newsym->id = lexeme;
    newsym->sym = NULL;

    if (!symbol_list) {
        symbol_list = newsym;
    } else {
        prevsym->sym = newsym;
    }
    return newsym;
}

/*
 * Match identifiers and reserved words.
 * Check if reserved word, if not add symbol to symbol table if needed.
 * Return token
 */
token idres_machine() {
    typedef enum {
        START, ALPHA_NUM
    } STATE;

    STATE state = START;
    char *lexeme;
    int lexerr = 0;
    token_node *cur_reserved_word;

    while (1) {
        switch(state) {
            case START:
                if (isalpha(*fptr)) {
                    state = ALPHA_NUM;
                    fptr++;
                } else {
                    /* no token matched */
                    return NONE_MATCHED;
                }
                break;

            case ALPHA_NUM:
                if (isalnum(*fptr)) {
                    fptr++;
                    if (fptr - bptr > LEX_MAX_ID) {
                        lexerr |= LEX_ERR_ID_TOO_LONG;
                    }
                    /* don't change state */
                } else {
                    lexeme = extract_lexeme(fptr, bptr);
                    bptr = fptr;

                    if (lexerr != 0) {
                        /* return lexerr token */
                        return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = lexerr };
                    }

                    /* check if reserved word */
                    for (cur_reserved_word = reserved_word_list; cur_reserved_word != NULL; cur_reserved_word = cur_reserved_word->node) {
                        if (!strcmp(cur_reserved_word->tok->lexeme, lexeme)) {
                            return *(cur_reserved_word->tok);
                        }
                    }

                    /* return id token */
                    return (token){ .lexeme = lexeme, .type = ID_TYPE, .attr.ptr = add_symbol(lexeme) };
                }
                break;
        }
    }
}

/*
 * Match miscellaneous single character lexemes and assignop.
 * Return token
 */
token misc_machine() {
    char *lexeme;

    switch (*fptr) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            /* return whitespace token */
            fptr++;
            bptr = fptr;
            lexeme = extract_lexeme(fptr, bptr);
            bptr = fptr;
            return (token){ .lexeme = lexeme, .type = WHITESPACE_TYPE, .attr.ptr = NULL };

        case ':':
            /* return colon token */
            fptr++;
            bptr = fptr;
            if (*fptr == '=') {
                fptr++;
                bptr = fptr;
                return (token){ .lexeme = make_lexeme(":="), .type = ASSIGNOP_TYPE, .attr.ptr = NULL };
            }
            return (token){ .lexeme = make_lexeme(":"), .type = COLON_TYPE, .attr.ptr = NULL };

        case ';':
            /* return colon token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme(";"), .type = SEMICOLON_TYPE, .attr.ptr = NULL };

        case ',':
            /* return comma token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme(","), .type = COMMA_TYPE, .attr.ptr = NULL };

        case '.':
            /* return period token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme("."), .type = PERIOD_TYPE, .attr.ptr = NULL };

        case '(':
            /* return left parenthesis token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme("("), .type = LPAREN_TYPE, .attr.ptr = NULL };

        case ')':
            /* return right parenthesis token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme(")"), .type = RPAREN_TYPE, .attr.ptr = NULL };

        case '[':
            /* return left bracket token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme("["), .type = LBRACKET_TYPE, .attr.ptr = NULL };

        case ']':
            /* return right bracket token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme("]"), .type = RBRACKET_TYPE, .attr.ptr = NULL };

        case '*':
            /* return mulop token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme("*"), .type = MULOP_TYPE, .attr.mulop = AND_MULOP };

        case '/':
            /* return mulop token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme("/"), .type = MULOP_TYPE, .attr.mulop = DIV_MULOP };

        case '+':
            /* return addop token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme("+"), .type = ADDOP_TYPE, .attr.addop = PLUS_ADDOP };

        case '-':
            /* return addop token */
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = make_lexeme("-"), .type = ADDOP_TYPE, .attr.addop = MINUS_ADDOP };

        default:
            /* no token matched */
            return NONE_MATCHED;
    }
}

/*
 * Match relops.
 * Return token
 */
token relop_machine() {
    typedef enum {
        START, LT, LE, NE, GT, GE, EQ
    } STATE;
    
    STATE state = START;
    char *lexeme;

    /* <= <> < > >= = */
    while (1) {
        switch(state) {
            case START:
                if (*fptr == '<') {
                    fptr++;
                    state = LT;
                } else if (*fptr == '>') {
                    fptr++;
                    state = GT;
                } else if (*fptr == '=') {
                    fptr++;
                    state = EQ;
                } else {
                    /* no token matched */
                    return NONE_MATCHED;
                }
                break;
                
            case LT:
                if (*fptr == '=') {
                    fptr++;
                    state = LE;
                } else if (*fptr == '>') {
                    fptr++;
                    state = NE;
                } else {
                    /* return LT RELOP token */
                    lexeme = extract_lexeme(fptr, bptr);
                    bptr = fptr;
                    return (token){ .lexeme = lexeme, .type = RELOP_TYPE, .attr.relop = LT_RELOP };
                }
                break;
                
            case LE:
                /* return LE RELOP token */
                lexeme = extract_lexeme(fptr, bptr);
                bptr = fptr;
                return (token){ .lexeme = lexeme, .type = RELOP_TYPE, .attr.relop = LE_RELOP };
                
            case NE:
                /* return NE RELOP token */
                lexeme = extract_lexeme(fptr, bptr);
                bptr = fptr;
                return (token){ .lexeme = lexeme, .type = RELOP_TYPE, .attr.relop = NE_RELOP };
                
            case GT:
                if (*fptr == '=') {
                    fptr++;
                    state = GE;
                } else {
                    /* return GT RELOP token */
                    lexeme = extract_lexeme(fptr, bptr);
                    bptr = fptr;
                    return (token){ .lexeme = lexeme, .type = RELOP_TYPE, .attr.relop = GT_RELOP };
                }
                break;
                
            case GE:
                /* return GE RELOP token */
                lexeme = extract_lexeme(fptr, bptr);
                bptr = fptr;
                return (token){ .lexeme = lexeme, .type = RELOP_TYPE, .attr.relop = GE_RELOP };
                
            case EQ:
                /* return EQ RELOP token */
                lexeme = extract_lexeme(fptr, bptr);
                bptr = fptr;
                return (token){ .lexeme = lexeme, .type = RELOP_TYPE, .attr.relop = EQ_RELOP };
        }
    }
}

/*
 * Match longreal numbers.
 * Return token
 */
token longreal_machine() {
    typedef enum {
        INTDOT, FRACTE, SIGN, EXP
    } STATE;
    
    STATE state = INTDOT;
    char *lexeme;
    char *tmpptr = fptr;
    int lexerr = 0;
    bool leadingzero = true;

    /* xx.yyEzz */
    while (1) {
        switch(state) {
            case INTDOT:
                if (*fptr == '0') {
                    if (leadingzero) {
                        /* lexical error: leading zero */
                        lexerr |= LEX_ERR_LEADING_ZERO;
                    }
                } else {
                    leadingzero = false;
                }
                if (*fptr >= '0' && *fptr <= '9') {
                    fptr++;
                    if (fptr - tmpptr > LEX_MAX_REALINT) {
                        /* lexical error: too long integer portion */
                        lexerr |= LEX_ERR_REALINT_TOO_LONG;
                    }
                    /* don't change state */
                } else if (*fptr == '.') {
                    /* "0.yyEzz" is not a leading zero */
                    if (*tmpptr == '0' && fptr - bptr == 1) {
                        lexerr &= !LEX_ERR_INT_TOO_LONG;
                    }

                    fptr++;
                    tmpptr = fptr;
                    leadingzero = true;
                    state = FRACTE;
                } else {
                    /* no token matched */
                    fptr = bptr;
                    return NONE_MATCHED;
                }
                break;
                
            case FRACTE:
                if (*fptr >= '0' && *fptr <= '9') {
                    fptr++;
                    if (fptr - tmpptr > LEX_MAX_FRAC) {
                        /* lexical error: too long fractional portion */
                        lexerr |= LEX_ERR_FRAC_TOO_LONG;
                    }
                    /* don't change state */
                } else if (*fptr == 'E' && fptr != tmpptr) {
                    /* must be at least one character long */
                    fptr++;
                    tmpptr = fptr;
                    leadingzero = true;
                    state = SIGN;
                } else {
                    /* no token matched */
                    fptr = bptr;
                    return NONE_MATCHED;
                }
                break;
                
            case SIGN:
                if (*fptr == '+') {
                    fptr++;
                    tmpptr = fptr;
                    state = EXP;
                } else if (*fptr == '-') {
                    fptr++;
                    tmpptr = fptr;
                    state = EXP;
                } else if (*fptr >= '0' && *fptr <= '9') {
                    state = EXP;
                } else {
                    /* no token matched */
                    fptr = bptr;
                    return NONE_MATCHED;
                }
                break;
                
            case EXP:
                if (*fptr == '0') {
                    if (leadingzero) {
                        /* lexical error: leading zero */
                        lexerr |= LEX_ERR_LEADING_ZERO;
                    }
                } else {
                    leadingzero = false;
                }
                if (*fptr >= '0' && *fptr <= '9') {
                    fptr++;
                    if (fptr - tmpptr > LEX_MAX_EXP) {
                        /* lexical error: too long exponent portion */
                        lexerr |= LEX_ERR_EXP_TOO_LONG;
                    }
                    state = EXP;
                } else if (fptr != tmpptr){
                    /* must match at least one character */

                    /* "xx.yyE0" is not a leading zero */
                    if (*tmpptr == '0' && fptr - tmpptr == 1) {
                        lexerr &= !LEX_ERR_INT_TOO_LONG;
                    }

                    lexeme = extract_lexeme(fptr, bptr);
                    bptr = fptr;

                    if (lexerr != 0) {
                        /* return lexerr token */
                        return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = lexerr };
                    }
                    
                    /* return longreal token */
                    return (token){ .lexeme = lexeme, .type = NUM_TYPE, .attr.num = LONGREAL_NUM };
                } else {
                    /* no token matched */
                    fptr = bptr;
                    return NONE_MATCHED;
                }
        }
    }
    
}

/*
 * Match real numbers.
 * Return token
 */
token real_machine() {
    
    typedef enum {
        INTDOT, FRACT
    } STATE;
    
    STATE state = INTDOT;
    char *lexeme;
    char *tmpptr = fptr;
    int lexerr = 0;
    bool leadingzero = true;

    /* xx.yy */
    while (1) {
        switch(state) {
            case INTDOT:
                if (*fptr == '0') {
                    if (leadingzero) {
                        /* lexical error: leading zero */
                        lexerr |= LEX_ERR_LEADING_ZERO;
                    }
                } else {
                    leadingzero = false;
                }
                if (*fptr >= '0' && *fptr <= '9') {
                    fptr++;
                    if (fptr - bptr > LEX_MAX_REALINT) {
                        /* lexical error: too long integer portion */
                        lexerr |= LEX_ERR_REALINT_TOO_LONG;
                    }
                    /* don't change state */
                } else if (*fptr == '.') {
                    /* "0.yy" is not a leading zero */
                    if (*tmpptr == '0' && fptr - bptr == 1) {
                        lexerr &= !LEX_ERR_INT_TOO_LONG;
                    }

                    fptr++;
                    tmpptr = fptr;
                    leadingzero = true;
                    state = FRACT;
                } else {
                    /* no token matched */
                    fptr = bptr;
                    return NONE_MATCHED;
                }
                break;
                
            case FRACT:
                if (*fptr >= '0' && *fptr <= '9') {
                    if (fptr - tmpptr == LEX_MAX_FRAC) {
                        /* lexical error: too long fractional portion */
                        lexerr |= LEX_ERR_FRAC_TOO_LONG;
                    }
                    fptr++;
                    /* don't change state */
                } else if (fptr != tmpptr){
                    /* must match at least one character */

                    lexeme = extract_lexeme(fptr, bptr);
                    bptr = fptr;
                    
                    if (lexerr != 0) {
                        /* return lexerr token */
                        return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = lexerr };
                    }
                    
                    /* return real token */
                    return (token){ .lexeme = lexeme, .type = NUM_TYPE, .attr.num = REAL_NUM };
                } else {
                    /* no token matched */
                    fptr = bptr;
                    return NONE_MATCHED;
                }
                break;
        }
    }
    
}


/*
 * Match int numbers.
 * Return token
 */
token int_machine() {
    int lexerr = 0;
    bool leadingzero = true;
    char *lexeme;
    
    while (1) {
        if (*fptr == '0') {
            if (leadingzero) {
                /* lexical error: leading zero */
                lexerr |= LEX_ERR_LEADING_ZERO;
            }
        } else {
            leadingzero = false;
        }
        if (*fptr >= '0' && *fptr <= '9') {
            if (fptr - bptr == LEX_MAX_INT) {
                /* lexical error: too long fractional portion */
                lexerr |= LEX_ERR_INT_TOO_LONG;
            }
            fptr++;
            /* don't change state */
        } else if(fptr != bptr) {
            /* must match at least one character */

            /* "0" is not a leading zero */
            if (fptr - bptr == 1) {
                lexerr &= !LEX_ERR_INT_TOO_LONG;
            }

            lexeme = extract_lexeme(fptr, bptr);
            bptr = fptr;

            if (lexerr != 0) {
                /* return lexerr token */
                return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = lexerr };
            }
            
            /* return int token */
            return (token){ .lexeme = lexeme, .type = NUM_TYPE, .attr.num = INTEGER_NUM };
        } else {
            /* no token matched */
            return NONE_MATCHED;
        }
    }
    
}
