typedef enum {
    LEXERR_TYPE,
    WHITESPACE_TYPE,
    PROGRAM_TYPE,
    VAR_TYPE,
    ARRAY_TYPE,
    INTEGER_TYPE,
    REAL_TYPE,
    PROCEDURE_TYPE,
    BEGIN_TYPE,
    END_TYPE,
    IF_TYPE,
    THEN_TYPE,
    ELSE_TYPE,
    WHILE_TYPE,
    DO_TYPE,
    NOT_TYPE,
    MULLOP_TYPE,
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
    DIV_RELOP,
    MOD_RELOP,
    AND_RELOP
} MULLOP_ATTR;

typedef enum {
    PLUS_RELOP,
    MINUS_RELOP,
    OR_RELOP
} ADDOP_ATTR;

typedef struct {
    char *lexeme;
    TOKEN_TYPE type;
    union {
        void* ptr;
        ADDOP_ATTR addop;
        MULLOP_ATTR mullop;
        RELOP_ATTR relop;
    } attr;
} token;




