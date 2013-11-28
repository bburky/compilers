#ifndef LEXEME_H
#define LEXEME_H

/* max line length */
#define LEX_MAX_LINE 72

/* max id char length */
#define LEX_MAX_ID 10

/* max int char length */
#define LEX_MAX_INT 10

/* max real clar length */
#define LEX_MAX_REALINT 5
#define LEX_MAX_FRAC 5
#define LEX_MAX_EXP 2

/* bitfield error types for lexical errors */
#define LEX_ERR_INT_TOO_LONG        (1 << 0)
#define LEX_ERR_REALINT_TOO_LONG    (1 << 1)
#define LEX_ERR_FRAC_TOO_LONG       (1 << 2)
#define LEX_ERR_EXP_TOO_LONG        (1 << 3)
#define LEX_ERR_LEADING_ZERO        (1 << 4)
#define LEX_ERR_ID_TOO_LONG         (1 << 5)
#define LEX_ERR_UNRECOGNIZED_SYMBOL (1 << 6)
#define NUM_LEX_ERR_TYPES           7

/* token for machines that do not match symbol */
#define NONE_MATCHED (token){ .lexeme = NULL, .type = NONE_TYPE, .attr.ptr = NULL }

/* define types and attributes with macros to autmatically generate the string and enum versions */
#define TOKEN_TYPE_TABLE \
X(ERR_TYPE)         /* 0  */ \
X(ERRSTAR_TYPE)     /* 1  */ \
X(ADDOP_TYPE)       /* 2  */ \
X(ARRAY_TYPE)       /* 3  */ \
X(ARRAY_RANGE_TYPE) /* 4  */ \
X(ASSIGNOP_TYPE)    /* 5  */ \
X(BEGIN_TYPE)       /* 6  */ \
X(CALL_TYPE)        /* 7  */ \
X(COLON_TYPE)       /* 8  */ \
X(COMMA_TYPE)       /* 9  */ \
X(DO_TYPE)          /* 10 */ \
X(ELSE_TYPE)        /* 11 */ \
X(END_TYPE)         /* 12 */ \
X(EOF_TYPE)         /* 13 */ \
X(ID_TYPE)          /* 14 */ \
X(IF_TYPE)          /* 15 */ \
X(INTEGER_TYPE)     /* 16 */ \
X(LBRACKET_TYPE)    /* 17 */ \
X(LEXERR_TYPE)      /* 18 */ \
X(LPAREN_TYPE)      /* 19 */ \
X(MINUS_TYPE)       /* 20 */ \
X(MULOP_TYPE)       /* 21 */ \
X(NONE_TYPE)        /* 22 */ \
X(NOT_TYPE)         /* 23 */ \
X(NUM_TYPE)         /* 24 */ \
X(OF_TYPE)          /* 25 */ \
X(PERIOD_TYPE)      /* 26 */ \
X(PLUS_TYPE)        /* 27 */ \
X(PROCEDURE_TYPE)   /* 28 */ \
X(PROGRAM_TYPE)     /* 29 */ \
X(RBRACKET_TYPE)    /* 30 */ \
X(REAL_TYPE)        /* 31 */ \
X(RELOP_TYPE)       /* 32 */ \
X(RPAREN_TYPE)      /* 33 */ \
X(SEMICOLON_TYPE)   /* 34 */ \
X(THEN_TYPE)        /* 35 */ \
X(VAR_TYPE)         /* 36 */ \
X(WHILE_TYPE)       /* 37 */ \
X(WHITESPACE_TYPE)  /* 38 */

#define RELOP_ATTR_TABLE \
X(LE_RELOP) \
X(GE_RELOP) \
X(GT_RELOP) \
X(LT_RELOP) \
X(EQ_RELOP) \
X(NE_RELOP)

#define MULOP_ATTR_TABLE \
X(AND_MULOP) \
X(DIV_MULOP) \
X(MOD_MULOP)

#define ADDOP_ATTR_TABLE \
X(MINUS_ADDOP) \
X(OR_ADDOP) \
X(PLUS_ADDOP) \

#define NUM_ATTR_TABLE \
X(INTEGER_NUM) \
X(LONGREAL_NUM) \
X(REAL_NUM)

#define X(a) a,
typedef enum {
    TOKEN_TYPE_TABLE
} TOKEN_TYPE;
typedef enum {
    RELOP_ATTR_TABLE
} RELOP_ATTR;
typedef enum {
    MULOP_ATTR_TABLE
} MULOP_ATTR;
typedef enum {
    ADDOP_ATTR_TABLE
} ADDOP_ATTR;
typedef enum {
    NUM_ATTR_TABLE
} NUM_ATTR;
#undef X

char *lex_err_name[NUM_LEX_ERR_TYPES][2];

char *token_type_name[39];

char *relop_attr_name[6];

char *mulop_attr_name[3];

char *addop_attr_name[3];

char *num_attr_name[3];

typedef struct {
    const char *lexeme;
    TOKEN_TYPE type;
    union {
        void* ptr;
        ADDOP_ATTR addop;
        MULOP_ATTR mulop;
        RELOP_ATTR relop;
        NUM_ATTR num;
        int errtype;
    } attr;
} token;

typedef struct symbol_node {
    const char *id;
    struct symbol_node *sym;
} symbol_node;

symbol_node *symbol_list;

typedef struct token_node {
    token *tok;
    struct token_node *node;
} token_node;

token_node *reserved_word_list;

#endif
