#define DEBUG

#define DEBUG_PRINT(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define DEBUG_TOKEN(fptr,bptr,str) printf("\n%s\nLexeme: %.*s\n", str, (int)(fptr-bptr), bptr)

/*
#define DEBUG_TOKEN(fptr,bptr,str) \
            do { if (DEBUG) fprintf(stderr, "\n%s\nLexeme:%.*s\n", str, (int)(fptr-bptr), bptr); } while (0)
*/

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
#define LEX_ERR_INT_TOO_LONG       (1 << 0)
#define LEX_ERR_REALINT_TOO_LONG   (1 << 1)
#define LEX_ERR_FRAC_TOO_LONG      (1 << 2)
#define LEX_ERR_EXP_TOO_LONG       (1 << 3)
#define LEX_ERR_LEADING_ZERO       (1 << 4)
#define LEX_ERR_ID_TOO_LONG        (1 << 5)

void init_lexer(const char*);
char* get_next_line();
token* get_next_token();

token* whitespace_machine();
token* relop_machine();
token* longreal_machine();
token* real_machine();
token* int_machine();
token* mulop_machine();
