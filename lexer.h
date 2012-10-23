#ifndef LEXER_H
#define LEXER_H

#include "lexeme.h"

inline char* extract_lexeme(const char* fptr, const char* bptr) {
    char *lexeme = malloc(fptr-bptr+1);
    if (!lexeme) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    strncpy(lexeme, bptr, fptr-bptr);
    lexeme[fptr-bptr] = '\0';
    return lexeme;
}

inline char* make_lexeme(const char* str) {
    char *lexeme = malloc(strlen(str)+1);
    if (!lexeme) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    strncpy(lexeme, str, strlen(str)+1);
    lexeme[strlen(str)] = '\0';
    return lexeme;
}

token misc_machine(void);

token relop_machine(void);

token longreal_machine(void);

token real_machine(void);

token int_machine(void);

#endif
