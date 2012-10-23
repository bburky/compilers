#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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
char *lex_err_name[] = {
    "INT_TOO_LONG",
    "REALINT_TOO_LONG",
    "FRAC_TOO_LONG",
    "EXP_TOO_LONG",
    "LEADING_ZERO",
    "ID_TOO_LONG",
    "UNRECOGNIZED_SYMBOL"
};
#undef X

extern inline char* extract_lexeme(const char* fptr, const char* bptr);

extern inline char* make_lexeme(const char* str);

token whitespace_machine() {
    char *lexeme;

    switch (*fptr) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            /* return whitespace token */
            fptr++;
            lexeme = extract_lexeme(fptr, bptr);
            bptr = fptr;
            return (token){ .lexeme = lexeme, .type = WHITESPACE_TYPE, .attr.ptr = NULL };
        default:
            /* no token matched */
            return NONE_MATCHED;
    }
}


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
                    state = EQ;
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

/* int : 123 */

/* longreal : exponent 1.0E(+|-)1 */
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
                } else {
                    lexeme = extract_lexeme(fptr, bptr);
                    bptr = fptr;
                    if (lexerr != 0) {
                        /* return lexerr token */
                        return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = lexerr };
                    }
                    /* return longreal token */
                    return (token){ .lexeme = lexeme, .type = LONGREAL_TYPE, .attr.ptr = NULL };
                }
        }
    }
    
}

/* real : 1.0 */
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
                    if (fptr - tmpptr > LEX_MAX_REALINT) {
                        /* lexical error: too long integer portion */
                        lexerr |= LEX_ERR_REALINT_TOO_LONG;
                    }
                    /* don't change state */
                } else if (*fptr == '.') {
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
                    if (fptr - tmpptr > LEX_MAX_FRAC) {
                        /* lexical error: too long fractional portion */
                        lexerr |= LEX_ERR_FRAC_TOO_LONG;
                    }
                    fptr++;
                    /* don't change state */
                } else {
                    lexeme = extract_lexeme(fptr, bptr);
                    bptr = fptr;
                    if (lexerr != 0) {
                        /* return lexerr token */
                        return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = lexerr };
                    }
                    /* return real token */
                    return (token){ .lexeme = lexeme, .type = REAL_TYPE, .attr.ptr = NULL };
                }
                break;
        }
    }
    
}


/* int : 123 */
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
            if (fptr - bptr > LEX_MAX_INT) {
                /* lexical error: too long fractional portion */
                lexerr |= LEX_ERR_INT_TOO_LONG;
            }
            fptr++;
            /* don't change state */
        } else if(fptr != bptr) {
            /* must match at least one character */
            lexeme = extract_lexeme(fptr, bptr);
            bptr = fptr;
            if (lexerr != 0) {
                /* return lexerr token */
                return (token){ .lexeme = lexeme, .type = LEXERR_TYPE, .attr.errtype = lexerr };
            }
            
            /* return int token */
            return (token){ .lexeme = lexeme, .type = INT_TYPE, .attr.ptr = NULL };
        } else {
            /* no token matched */
            return NONE_MATCHED;
        }
    }
    
}

token mulop_machine() {
    if (*fptr == '*') {
        fptr++;
        bptr = fptr;
        return (token){ .lexeme = make_lexeme("*"), .type = MULOP_TYPE, .attr.mulop = AND_MULOP };
    } else if (*fptr == '/') {
        fptr++;
        bptr = fptr;
        return (token){ .lexeme = make_lexeme("/"), .type = MULOP_TYPE, .attr.mulop = DIV_MULOP };
    } else if (!strncmp(fptr, "mod", 3)) {
        fptr += 3;
        bptr = fptr;
        return (token){ .lexeme = make_lexeme("mod"), .type = MULOP_TYPE, .attr.mulop = MOD_MULOP };
    } else if (!strncmp(fptr, "div", 3)) {
        fptr += 3;
        bptr = fptr;
        return (token){ .lexeme = make_lexeme("div"), .type = MULOP_TYPE, .attr.mulop = DIV_MULOP };
    } else if (!strncmp(fptr, "and", 3)) {
        fptr += 3;
        bptr = fptr;
        return (token){ .lexeme = make_lexeme("and"), .type = MULOP_TYPE, .attr.mulop = AND_MULOP };
    } else {
        /* no token matched */
        return NONE_MATCHED;
    }
}

token addop_machine() {
    if (*fptr == '+') {
        fptr++;
        bptr = fptr;
        return (token){ .lexeme = make_lexeme("+"), .type = ADDOP_TYPE, .attr.addop = PLUS_ADDOP };
    } else if (*fptr == '-') {
        fptr++;
        bptr = fptr;
        return (token){ .lexeme = make_lexeme("-"), .type = ADDOP_TYPE, .attr.addop = MINUS_ADDOP };
    } else if (!strncmp(fptr, "or", 2)) {
        fptr += 2;
        bptr = fptr;
        return (token){ .lexeme = make_lexeme("or"), .type = ADDOP_TYPE, .attr.addop = OR_ADDOP };
    } else {
        /* no token matched */
        return NONE_MATCHED;
    }
}
