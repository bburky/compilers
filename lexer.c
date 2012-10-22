#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "compiler.h"

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
#undef X

token whitespace_machine() {
    switch (*fptr) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            /* return whitespace token */
            DEBUG_TOKEN(fptr, bptr, "whitespace token");
            fptr++;
            bptr = fptr;
            return (token){ .lexeme = NULL, .type = WHITESPACE_TYPE, .attr.ptr = NULL };
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
                    /* Return LT RELOP token */
                    DEBUG_TOKEN(fptr, bptr, "relop token");
                    bptr = fptr;
                    return (token){ .lexeme = NULL, .type = RELOP_TYPE, .attr.relop = LT_RELOP };
                }
                break;
                
            case LE:
                /* Return LE RELOP token */
                DEBUG_TOKEN(fptr, bptr, "relop token");
                bptr = fptr;
                return (token){ .lexeme = NULL, .type = RELOP_TYPE, .attr.relop = LE_RELOP };
                
            case NE:
                /* Return NE RELOP token */
                DEBUG_TOKEN(fptr, bptr, "relop token");
                bptr = fptr;
                return (token){ .lexeme = NULL, .type = RELOP_TYPE, .attr.relop = NE_RELOP };
                
            case GT:
                if (*fptr == '=') {
                    fptr++;
                    state = EQ;
                } else {
                    /* Return GT RELOP token */
                    DEBUG_TOKEN(fptr, bptr, "relop token");
                    bptr = fptr;
                    return (token){ .lexeme = NULL, .type = RELOP_TYPE, .attr.relop = GT_RELOP };
                }
                break;
                
            case GE:
                /* Return GE RELOP token */
                DEBUG_TOKEN(fptr, bptr, "relop token");
                bptr = fptr;
                return (token){ .lexeme = NULL, .type = RELOP_TYPE, .attr.relop = GE_RELOP };
                
            case EQ:
                /* Return EQ RELOP token */
                DEBUG_TOKEN(fptr, bptr, "relop token");
                bptr = fptr;
                return (token){ .lexeme = NULL, .type = RELOP_TYPE, .attr.relop = EQ_RELOP };
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
                    state = EXP;
                } else if (*fptr == '-') {
                    fptr++;
                    state = EXP;
                } else if (*fptr >= '0' && *fptr <= '9') {
                    if (fptr - tmpptr > LEX_MAX_FRAC) {
                        /* lexical error: too long fractional portion */
                        lexerr |= LEX_ERR_FRAC_TOO_LONG;
                    }
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
                    if (fptr - tmpptr > LEX_MAX_FRAC) {
                        /* lexical error: too long fractional portion */
                        lexerr |= LEX_ERR_FRAC_TOO_LONG;
                    }
                    state = EXP;
                } else {
                    if (lexerr != 0) {
                        /* return lexerr token */
                        return NONE_MATCHED;
                    }
                    /* return longreal token */
                    DEBUG_TOKEN(fptr, bptr, "longreal token");
                    bptr = fptr;
                    return (token){ .lexeme = NULL, .type = LONGREAL_TYPE, .attr.ptr = NULL };
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
                if (*fptr == '0') {
                    if (leadingzero) {
                        /* lexical error: leading zero */
                        lexerr |= LEX_ERR_LEADING_ZERO;
                    }
                } else {
                    leadingzero = false;
                }
                if (*fptr >= '0' && *fptr <= '9') {
                    if (fptr - tmpptr > LEX_MAX_FRAC) {
                        /* lexical error: too long fractional portion */
                        lexerr |= LEX_ERR_FRAC_TOO_LONG;
                    }
                    fptr++;
                    /* don't change state */
                } else {
                    /* matched real */
                    if (lexerr != 0) {
                        /* return lexerr token */
                        return NONE_MATCHED;
                    }
                    
                    /* return real token */
                    DEBUG_TOKEN(fptr, bptr, "real token");
                    bptr = fptr;
                    return (token){ .lexeme = NULL, .type = REAL_TYPE, .attr.ptr = NULL };
                }
                break;
        }
    }
    
}


/* int : 123 */
token int_machine() {
    int lexerr = 0;
    bool leadingzero = true;
    
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
            /* matched real */
            if (lexerr != 0) {
                /* return lexerr token */
                return NONE_MATCHED;
            }
            
            /* return int token */
            DEBUG_TOKEN(fptr, bptr, "int token");
            bptr = fptr;
            return (token){ .lexeme = NULL, .type = INT_TYPE, .attr.ptr = NULL };
        } else {
            /* no token matched */
            return NONE_MATCHED;
        }
    }
    
}

token mulop_machine() {
    if (*fptr == '*') {
        fptr++;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        bptr = fptr;
        return (token){ .lexeme = NULL, .type = MULOP_TYPE, .attr.mulop = AND_MULOP };
    } else if (*fptr == '/') {
        fptr++;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        bptr = fptr;
        return (token){ .lexeme = NULL, .type = MULOP_TYPE, .attr.mulop = DIV_MULOP };
    } else if (!strncmp(fptr, "mod", 3)) {
        fptr += 3;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        bptr = fptr;
        return (token){ .lexeme = NULL, .type = MULOP_TYPE, .attr.mulop = MOD_MULOP };
    } else if (!strncmp(fptr, "div", 3)) {
        fptr += 3;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        bptr = fptr;
        return (token){ .lexeme = NULL, .type = MULOP_TYPE, .attr.mulop = DIV_MULOP };
    } else if (!strncmp(fptr, "and", 3)) {
        fptr += 3;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        bptr = fptr;
        return (token){ .lexeme = NULL, .type = MULOP_TYPE, .attr.mulop = AND_MULOP };
    } else {
        /* no token matched */
        return NONE_MATCHED;
    }
}


token addop_machine() {
    if (*fptr == '+') {
        fptr++;
        DEBUG_TOKEN(fptr, bptr, "addop token");
        bptr = fptr;
        return (token){ .lexeme = NULL, .type = ADDOP_TYPE, .attr.addop = PLUS_ADDOP };
    } else if (*fptr == '-') {
        fptr++;
        DEBUG_TOKEN(fptr, bptr, "addop token");
        bptr = fptr;
        return (token){ .lexeme = NULL, .type = ADDOP_TYPE, .attr.addop = MINUS_ADDOP };
    } else if (!strncmp(fptr, "or", 2)) {
        fptr += 2;
        DEBUG_TOKEN(fptr, bptr, "addop token");
        bptr = fptr;
        return (token){ .lexeme = NULL, .type = ADDOP_TYPE, .attr.addop = OR_ADDOP };
    } else {
        /* no token matched */
        return NONE_MATCHED;
    }
}