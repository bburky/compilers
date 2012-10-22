#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexeme.h"
#include "compiler.h"

const char *input_filename;
FILE *input_file, *listing_file, *token_file, *symbol_file;
char line[80]; /* 78 chars + \n\0 */

/* Forward and back pointers used in lexer */
char *fptr = NULL, *bptr = NULL;

int main(int argc, const char *argv[]) {
    token *tok;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s source_file_name\n", argv[0]);
        exit(1);
    }

    /* argv[1] is filename */
    init_lexer(argv[1]);


    while (1) {
        tok = get_next_token();
        if (!tok) {
            /* TODO: EOF token */
            break;
        }
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
        fprintf(stderr, "Line greater than 78 chars\n");
        fprintf(stderr, "Line is %d chars long: %s\n", (int) strlen(line), line);
        exit(1);
    }
    return line;
}

token* get_next_token() {
    token *(*machines[])() = {whitespace_machine, relop_machine, mulop_machine, longreal_machine, real_machine, int_machine};
    // token *(*current_machine)() = *machines;
    int current_machine;
    token *matched_token;

    /* if first line or end of current line */
    if (fptr == NULL || *fptr == '\0') {
        if (!get_next_line()) {
            /* return EOF token */
            return 0;
        }
        fptr = line;
        bptr = line;
    }

    /* call each machine in order */
    for (current_machine = 0; current_machine < 6; current_machine++) {
        matched_token = machines[current_machine]();
        if(matched_token) {
            return matched_token;
        }
    }

    /* no token matched */
    /* lexerr */
    printf("\nNo token matched\n");
    return NULL;
}

token* whitespace_machine() {
    token *whitespace_token;

    switch (*fptr) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        /* return whitespace token */
        DEBUG_TOKEN(fptr, bptr, "whitespace token");
        whitespace_token = malloc(sizeof(token));
        if (!whitespace_token) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }
        whitespace_token->lexeme = NULL;
        whitespace_token->type = WHITESPACE_TYPE;
        whitespace_token->attr.ptr = NULL;
        fptr++;
        bptr = fptr;
        return whitespace_token;
    default:
        /* no token matched */
        return NULL;
    }
}


token* relop_machine() {
    typedef enum {
        START, LT, LE, NE, GT, GE, EQ
    } STATE;

    STATE state = START;
    token *relop_token;

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
                return NULL;
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
                relop_token = malloc(sizeof(token));
                if (!relop_token) {
                    fprintf(stderr, "Out of memory");
                    exit(1);
                }
                relop_token->lexeme = NULL;
                relop_token->type = RELOP_TYPE;
                relop_token->attr.relop = LT_RELOP;
                bptr = fptr;
                return relop_token;
            }
            break;

        case LE:
            /* Return LE RELOP token */
            DEBUG_TOKEN(fptr, bptr, "relop token");
            relop_token = malloc(sizeof(token));
            if (!relop_token) {
                fprintf(stderr, "Out of memory");
                exit(1);
            }
            relop_token->lexeme = NULL;
            relop_token->type = RELOP_TYPE;
            relop_token->attr.relop = LE_RELOP;
            bptr = fptr;
            return relop_token;

        case NE:
            /* Return NE RELOP token */
            DEBUG_TOKEN(fptr, bptr, "relop token");
            relop_token = malloc(sizeof(token));
            if (!relop_token) {
                fprintf(stderr, "Out of memory");
                exit(1);
            }
            relop_token->lexeme = NULL;
            relop_token->type = RELOP_TYPE;
            relop_token->attr.relop = NE_RELOP;
            bptr = fptr;
            return relop_token;

        case GT:
            if (*fptr == '=') {
                fptr++;
                state = EQ;
            } else {
                /* Return GT RELOP token */
                DEBUG_TOKEN(fptr, bptr, "relop token");
                relop_token = malloc(sizeof(token));
                if (!relop_token) {
                    fprintf(stderr, "Out of memory");
                    exit(1);
                }
                relop_token->lexeme = NULL;
                relop_token->type = RELOP_TYPE;
                relop_token->attr.relop = GT_RELOP;
                bptr = fptr;
                return relop_token;
            }
            break;

        case GE:
            /* Return GE RELOP token */
            DEBUG_TOKEN(fptr, bptr, "relop token");
            relop_token = malloc(sizeof(token));
            if (!relop_token) {
                fprintf(stderr, "Out of memory");
                exit(1);
            }
            relop_token->lexeme = NULL;
            relop_token->type = RELOP_TYPE;
            relop_token->attr.relop = GE_RELOP;
            bptr = fptr;
            return relop_token;

        case EQ:
            /* Return EQ RELOP token */
            DEBUG_TOKEN(fptr, bptr, "relop token");
            relop_token = malloc(sizeof(token));
            if (!relop_token) {
                fprintf(stderr, "Out of memory");
                exit(1);
            }
            relop_token->lexeme = NULL;
            relop_token->type = RELOP_TYPE;
            relop_token->attr.relop = EQ_RELOP;
            bptr = fptr;
            return relop_token;
        }
    }
}

/* int : 123 */

/* longreal : exponent 1.0E(+|-)1 */
token* longreal_machine() {
    typedef enum {
        INTDOT, FRACTE, SIGN, EXP
    } STATE;

    STATE state = INTDOT;
    char *tmpptr = fptr;
    int lexerr = 0;
    bool leadingzero = true;
    token *longreal_token;


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
                return NULL;
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
                return NULL;
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
                return NULL;
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
                    return NULL;
                }
                /* return longreal token */
                DEBUG_TOKEN(fptr, bptr, "longreal token");
                longreal_token = malloc(sizeof(token));
                if (!longreal_token) {
                    fprintf(stderr, "Out of memory");
                    exit(1);
                }
                longreal_token->lexeme = NULL;
                longreal_token->type = LONGREAL_TYPE;
                longreal_token->attr.ptr = NULL;
                bptr = fptr;
                return longreal_token;
            }
        }
    }

}

/* real : 1.0 */
token* real_machine() {

    typedef enum {
        INTDOT, FRACT
    } STATE;

    STATE state = INTDOT;
    char *tmpptr = fptr;
    int lexerr = 0;
    bool leadingzero = true;
    token *real_token;

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
                return NULL;
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
                    return NULL;
                }
                
                /* return real token */
                DEBUG_TOKEN(fptr, bptr, "real token");
                real_token = malloc(sizeof(token));
                if (!real_token) {
                    fprintf(stderr, "Out of memory");
                    exit(1);
                }
                real_token->lexeme = NULL;
                real_token->type = REAL_TYPE;
                real_token->attr.ptr = NULL;
                bptr = fptr;
                return real_token;
            }
            break;
        }
    }

}


/* int : 123 */
token* int_machine() {
    int lexerr = 0;
    bool leadingzero = true;
    token *int_token;
    
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
                return NULL;
            }
            
            /* return int token */
            DEBUG_TOKEN(fptr, bptr, "int token");
            int_token = malloc(sizeof(token));
            if (!int_token) {
                fprintf(stderr, "Out of memory");
                exit(1);
            }
            int_token->lexeme = NULL;
            int_token->type = INT_TYPE;
            int_token->attr.ptr = NULL;
            bptr = fptr;
            return int_token;
        } else {
            /* no token matched */
            return NULL;
        }
    }

}

token* mulop_machine() {
    token *mulop_token;
    
    if (*fptr == '*') {
        fptr++;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        mulop_token = malloc(sizeof(token));
        if (!mulop_token) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }
        mulop_token->lexeme = NULL;
        mulop_token->type = MULOP_TYPE;
        mulop_token->attr.mulop = AND_MULOP;
        bptr = fptr;
        return mulop_token;
    } else if (*fptr == '/') {
        fptr++;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        mulop_token = malloc(sizeof(token));
        if (!mulop_token) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }
        mulop_token->lexeme = NULL;
        mulop_token->type = MULOP_TYPE;
        mulop_token->attr.mulop = DIV_MULOP;
        bptr = fptr;
        return mulop_token;
    } else if (!strncmp(fptr, "mod", 3)) {
        fptr += 3;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        mulop_token = malloc(sizeof(token));
        if (!mulop_token) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }
        mulop_token->lexeme = NULL;
        mulop_token->type = MULOP_TYPE;
        mulop_token->attr.mulop = MOD_MULOP;
        bptr = fptr;
        return mulop_token;
    } else if (!strncmp(fptr, "div", 3)) {
        fptr += 3;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        mulop_token = malloc(sizeof(token));
        if (!mulop_token) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }
        mulop_token->lexeme = NULL;
        mulop_token->type = MULOP_TYPE;
        mulop_token->attr.mulop = DIV_MULOP;
        bptr = fptr;
        return mulop_token;
    } else if (!strncmp(fptr, "and", 3)) {
        fptr += 3;
        DEBUG_TOKEN(fptr, bptr, "mulop token");
        mulop_token = malloc(sizeof(token));
        if (!mulop_token) {
            fprintf(stderr, "Out of memory");
            exit(1);
        }
        mulop_token->lexeme = NULL;
        mulop_token->type = MULOP_TYPE;
        mulop_token->attr.mulop = AND_MULOP;
        bptr = fptr;
        return mulop_token;
    } else {
        /* no token matched */
        return NULL;
    }
}
