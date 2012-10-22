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
    ADD_TYPE,
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




