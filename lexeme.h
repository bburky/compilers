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
X(ADDOP_TYPE) \
X(ARRAY_TYPE) \
X(ASSIGNOP_TYPE) \
X(BEGIN_TYPE) \
X(COLON_TYPE) \
X(COMMA_TYPE) \
X(DO_TYPE) \
X(ELSE_TYPE) \
X(END_TYPE) \
X(EOF_TYPE) \
X(FUNCTION_TYPE) \
X(ID_TYPE) \
X(IF_TYPE) \
X(INTEGER_TYPE) \
X(LBRACKET_TYPE) \
X(LEXERR_TYPE) \
X(LPAREN_TYPE) \
X(MULOP_TYPE) \
X(NONE_TYPE) \
X(NOT_TYPE) \
X(NUM_TYPE) \
X(OF_TYPE) \
X(PERIOD_TYPE) \
X(PROCEDURE_TYPE) \
X(PROGRAM_TYPE) \
X(RBRACKET_TYPE) \
X(REAL_TYPE) \
X(RELOP_TYPE) \
X(RPAREN_TYPE) \
X(SEMICOLON_TYPE) \
X(THEN_TYPE) \
X(VAR_TYPE) \
X(WHILE_TYPE) \
X(WHITESPACE_TYPE)

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

char *lex_err_name[][2];

char *token_type_name[34];

char *relop_attr_name[];

char *mulop_attr_name[];

char *addop_attr_name[];

char *num_attr_name[];

typedef struct {
    char *lexeme;
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
