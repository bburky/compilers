#define DEBUG

#define DEBUG_PRINT(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define DEBUG_TOKEN(fptr,bptr,str) printf("\n%s\nLexeme: %.*s\n", str, (int)(fptr-bptr), bptr)

/*
#define DEBUG_TOKEN(fptr,bptr,str) \
            do { if (DEBUG) fprintf(stderr, "\n%s\nLexeme:%.*s\n", str, (int)(fptr-bptr), bptr); } while (0)
*/

void init_lexer(const char*);
char* get_next_line();
token* get_next_token();

token* whitespace_machine();
token* relop_machine();
token* longreal_machine();
token* real_machine();
token* int_machine();
token* mulop_machine();
token* addop_machine();
