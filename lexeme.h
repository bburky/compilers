
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

typedef enum {
    LEXERR_TYPE,
    WHITESPACE_TYPE,
    PROGRAM_TYPE,
    VAR_TYPE,
    ARRAY_TYPE,
    INTEGER_TYPE,
    LONGREAL_TYPE,
    REAL_TYPE,
    INT_TYPE,
    PROCEDURE_TYPE,
    BEGIN_TYPE,
    END_TYPE,
    IF_TYPE,
    THEN_TYPE,
    ELSE_TYPE,
    WHILE_TYPE,
    DO_TYPE,
    NOT_TYPE,
    MULOP_TYPE,
    ADDOP_TYPE,
    RELOP_TYPE
} TOKEN_TYPE;

typedef enum {
    LE_RELOP,
    GE_RELOP,
    GT_RELOP,
    LT_RELOP,
    EQ_RELOP,
    NE_RELOP
} RELOP_ATTR;

typedef enum {
    DIV_MULOP,
    MOD_MULOP,
    AND_MULOP
} MULOP_ATTR;   

typedef enum {
    PLUS_ADDOP,
    MINUS_ADDOP,
    OR_ADDOP
} ADDOP_ATTR;

typedef struct {
    char *lexeme;
    TOKEN_TYPE type;
    union {
        void* ptr;
        ADDOP_ATTR addop;
        MULOP_ATTR mulop;
        RELOP_ATTR relop;
        int errtype;
    } attr;
} token;




