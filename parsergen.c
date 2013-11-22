#include "compiler.h"
#include "parser.h"
#include "parsergen.h"

void parse_program() {
	// first(program): PROGRAM
	// follow(program): EOF
    
	DEBUG_PRINT(("Parsing program\n"));
    
	TOKEN_TYPE expected[] = {PROGRAM_TYPE};
	TOKEN_TYPE synch_set[] = {PROGRAM_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case PROGRAM_TYPE:
            // Grammar production: PROGRAM ID LPAREN identifier_list RPAREN SEMICOLON program_2
            match(PROGRAM_TYPE);
            match(ID_TYPE);
            match(LPAREN_TYPE);
            parse_identifier_list();
            match(RPAREN_TYPE);
            match(SEMICOLON_TYPE);
            parse_program_2();
            return;
        default:
            error();
            return;
	}
}

void parse_program_2() {
	// first(program_2): VAR, BEGIN, PROCEDURE
	// follow(program_2): EOF
    
	DEBUG_PRINT(("Parsing program_2\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE, BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {VAR_TYPE, BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: compound_statement PERIOD
            parse_compound_statement();
            match(PERIOD_TYPE);
            return;
        case VAR_TYPE:
            // Grammar production: declarations program_3
            parse_declarations();
            parse_program_3();
            return;
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declarations compound_statement PERIOD
            parse_subprogram_declarations();
            parse_compound_statement();
            match(PERIOD_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_program_3() {
	// first(program_3): BEGIN, PROCEDURE
	// follow(program_3): EOF
    
	DEBUG_PRINT(("Parsing program_3\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: compound_statement PERIOD
            parse_compound_statement();
            match(PERIOD_TYPE);
            return;
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declarations compound_statement PERIOD
            parse_subprogram_declarations();
            parse_compound_statement();
            match(PERIOD_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_identifier_list() {
	// first(identifier_list): ID
	// follow(identifier_list): COLON, RPAREN
    
	DEBUG_PRINT(("Parsing identifier_list\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, COLON_TYPE, EOF_TYPE, ID_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
            // Grammar production: ID identifier_list_2
            match(ID_TYPE);
            parse_identifier_list_2();
            return;
        default:
            error();
            return;
	}
}

void parse_identifier_list_2() {
	// first(identifier_list_2): EPSILON, COMMA
	// follow(identifier_list_2): COLON, RPAREN
    
	DEBUG_PRINT(("Parsing identifier_list_2\n"));
    
	TOKEN_TYPE expected[] = {COLON_TYPE, COMMA_TYPE, RPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {COLON_TYPE, COMMA_TYPE, EOF_TYPE, RPAREN_TYPE};
    
	switch(tok.type) {
        case COMMA_TYPE:
            // Grammar production: COMMA ID identifier_list_2
            match(COMMA_TYPE);
            match(ID_TYPE);
            parse_identifier_list_2();
            return;
        case COLON_TYPE:
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        default:
            error();
            return;
	}
}

void parse_declarations() {
	// first(declarations): VAR
	// follow(declarations): BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing declarations\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE};
	TOKEN_TYPE synch_set[] = {VAR_TYPE, BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case VAR_TYPE:
            // Grammar production: VAR identifier_list COLON type SEMICOLON declarations_2
            match(VAR_TYPE);
            parse_identifier_list();
            match(COLON_TYPE);
            parse_type();
            match(SEMICOLON_TYPE);
            parse_declarations_2();
            return;
        default:
            error();
            return;
	}
}

void parse_declarations_2() {
	// first(declarations_2): VAR, EPSILON
	// follow(declarations_2): BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing declarations_2\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE, BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {VAR_TYPE, BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
        case PROCEDURE_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case VAR_TYPE:
            // Grammar production: VAR identifier_list COLON type SEMICOLON declarations_2
            match(VAR_TYPE);
            parse_identifier_list();
            match(COLON_TYPE);
            parse_type();
            match(SEMICOLON_TYPE);
            parse_declarations_2();
            return;
        default:
            error();
            return;
	}
}

void parse_type() {
	// first(type): REAL, INTEGER, ARRAY
	// follow(type): RPAREN, SEMICOLON
    
	DEBUG_PRINT(("Parsing type\n"));
    
	TOKEN_TYPE expected[] = {REAL_TYPE, INTEGER_TYPE, ARRAY_TYPE};
	TOKEN_TYPE synch_set[] = {REAL_TYPE, RPAREN_TYPE, EOF_TYPE, SEMICOLON_TYPE, INTEGER_TYPE, ARRAY_TYPE};
    
	switch(tok.type) {
        case ARRAY_TYPE:
            // Grammar production: ARRAY LBRACKET NUM ARRAY_RANGE NUM RBRACKET OF standard_type
            match(ARRAY_TYPE);
            match(LBRACKET_TYPE);
            match(NUM_TYPE);
            match(ARRAY_RANGE_TYPE);
            match(NUM_TYPE);
            match(RBRACKET_TYPE);
            match(OF_TYPE);
            parse_standard_type();
            return;
        case REAL_TYPE:
        case INTEGER_TYPE:
            // Grammar production: standard_type
            parse_standard_type();
            return;
        default:
            error();
            return;
	}
}

void parse_standard_type() {
	// first(standard_type): REAL, INTEGER
	// follow(standard_type): RPAREN, SEMICOLON
    
	DEBUG_PRINT(("Parsing standard_type\n"));
    
	TOKEN_TYPE expected[] = {REAL_TYPE, INTEGER_TYPE};
	TOKEN_TYPE synch_set[] = {REAL_TYPE, INTEGER_TYPE, RPAREN_TYPE, EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case INTEGER_TYPE:
            // Grammar production: INTEGER
            match(INTEGER_TYPE);
            return;
        case REAL_TYPE:
            // Grammar production: REAL
            match(REAL_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_subprogram_declarations() {
	// first(subprogram_declarations): PROCEDURE
	// follow(subprogram_declarations): BEGIN
    
	DEBUG_PRINT(("Parsing subprogram_declarations\n"));
    
	TOKEN_TYPE expected[] = {PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declaration SEMICOLON subprogram_declarations_2
            parse_subprogram_declaration();
            match(SEMICOLON_TYPE);
            parse_subprogram_declarations_2();
            return;
        default:
            error();
            return;
	}
}

void parse_subprogram_declarations_2() {
	// first(subprogram_declarations_2): EPSILON, PROCEDURE
	// follow(subprogram_declarations_2): BEGIN
    
	DEBUG_PRINT(("Parsing subprogram_declarations_2\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declaration SEMICOLON subprogram_declarations_2
            parse_subprogram_declaration();
            match(SEMICOLON_TYPE);
            parse_subprogram_declarations_2();
            return;
        default:
            error();
            return;
	}
}

void parse_subprogram_declaration() {
	// first(subprogram_declaration): PROCEDURE
	// follow(subprogram_declaration): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration\n"));
    
	TOKEN_TYPE expected[] = {PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {SEMICOLON_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_head subprogram_declaration_2
            parse_subprogram_head();
            parse_subprogram_declaration_2();
            return;
        default:
            error();
            return;
	}
}

void parse_subprogram_declaration_2() {
	// first(subprogram_declaration_2): VAR, BEGIN, PROCEDURE
	// follow(subprogram_declaration_2): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration_2\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE, BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {VAR_TYPE, BEGIN_TYPE, SEMICOLON_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: compound_statement
            parse_compound_statement();
            return;
        case VAR_TYPE:
            // Grammar production: declarations subprogram_declaration_3
            parse_declarations();
            parse_subprogram_declaration_3();
            return;
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declarations compound_statement
            parse_subprogram_declarations();
            parse_compound_statement();
            return;
        default:
            error();
            return;
	}
}

void parse_subprogram_declaration_3() {
	// first(subprogram_declaration_3): BEGIN, PROCEDURE
	// follow(subprogram_declaration_3): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration_3\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, SEMICOLON_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: compound_statement
            parse_compound_statement();
            return;
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declarations compound_statement
            parse_subprogram_declarations();
            parse_compound_statement();
            return;
        default:
            error();
            return;
	}
}

void parse_subprogram_head() {
	// first(subprogram_head): PROCEDURE
	// follow(subprogram_head): VAR, BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing subprogram_head\n"));
    
	TOKEN_TYPE expected[] = {PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {VAR_TYPE, BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case PROCEDURE_TYPE:
            // Grammar production: PROCEDURE ID subprogram_head_2
            match(PROCEDURE_TYPE);
            match(ID_TYPE);
            parse_subprogram_head_2();
            return;
        default:
            error();
            return;
	}
}

void parse_subprogram_head_2() {
	// first(subprogram_head_2): LPAREN, SEMICOLON
	// follow(subprogram_head_2): VAR, BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing subprogram_head_2\n"));
    
	TOKEN_TYPE expected[] = {SEMICOLON_TYPE, LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, SEMICOLON_TYPE, EOF_TYPE, LPAREN_TYPE, VAR_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case SEMICOLON_TYPE:
            // Grammar production: SEMICOLON
            match(SEMICOLON_TYPE);
            return;
        case LPAREN_TYPE:
            // Grammar production: arguments SEMICOLON
            parse_arguments();
            match(SEMICOLON_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_arguments() {
	// first(arguments): LPAREN
	// follow(arguments): SEMICOLON
    
	DEBUG_PRINT(("Parsing arguments\n"));
    
	TOKEN_TYPE expected[] = {LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {SEMICOLON_TYPE, EOF_TYPE, LPAREN_TYPE};
    
	switch(tok.type) {
        case LPAREN_TYPE:
            // Grammar production: LPAREN parameter_list RPAREN
            match(LPAREN_TYPE);
            parse_parameter_list();
            match(RPAREN_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_parameter_list() {
	// first(parameter_list): ID
	// follow(parameter_list): RPAREN
    
	DEBUG_PRINT(("Parsing parameter_list\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE, ID_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
            // Grammar production: identifier_list COLON type parameter_list_2
            parse_identifier_list();
            match(COLON_TYPE);
            parse_type();
            parse_parameter_list_2();
            return;
        default:
            error();
            return;
	}
}

void parse_parameter_list_2() {
	// first(parameter_list_2): EPSILON, SEMICOLON
	// follow(parameter_list_2): RPAREN
    
	DEBUG_PRINT(("Parsing parameter_list_2\n"));
    
	TOKEN_TYPE expected[] = {RPAREN_TYPE, SEMICOLON_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case SEMICOLON_TYPE:
            // Grammar production: SEMICOLON identifier_list COLON type parameter_list_2
            match(SEMICOLON_TYPE);
            parse_identifier_list();
            match(COLON_TYPE);
            parse_type();
            parse_parameter_list_2();
            return;
        default:
            error();
            return;
	}
}

void parse_compound_statement() {
	// first(compound_statement): BEGIN
	// follow(compound_statement): END, PERIOD, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing compound_statement\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, PERIOD_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: BEGIN compound_statement_2
            match(BEGIN_TYPE);
            parse_compound_statement_2();
            return;
        default:
            error();
            return;
	}
}

void parse_compound_statement_2() {
	// first(compound_statement_2): BEGIN, END, WHILE, CALL, ID, IF
	// follow(compound_statement_2): END, PERIOD, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing compound_statement_2\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, END_TYPE, WHILE_TYPE, CALL_TYPE, ID_TYPE, IF_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, PERIOD_TYPE, ELSE_TYPE, WHILE_TYPE, CALL_TYPE, ID_TYPE, IF_TYPE};
    
	switch(tok.type) {
        case END_TYPE:
            // Grammar production: END
            match(END_TYPE);
            return;
        case BEGIN_TYPE:
        case WHILE_TYPE:
        case CALL_TYPE:
        case ID_TYPE:
        case IF_TYPE:
            // Grammar production: optional_statements END
            parse_optional_statements();
            match(END_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_optional_statements() {
	// first(optional_statements): BEGIN, WHILE, CALL, ID, IF
	// follow(optional_statements): END
    
	DEBUG_PRINT(("Parsing optional_statements\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, CALL_TYPE, WHILE_TYPE, ID_TYPE, IF_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, CALL_TYPE, EOF_TYPE, END_TYPE, ID_TYPE, WHILE_TYPE, IF_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
        case CALL_TYPE:
        case WHILE_TYPE:
        case ID_TYPE:
        case IF_TYPE:
            // Grammar production: statement_list
            parse_statement_list();
            return;
        default:
            error();
            return;
	}
}

void parse_statement_list() {
	// first(statement_list): BEGIN, WHILE, CALL, ID, IF
	// follow(statement_list): END
    
	DEBUG_PRINT(("Parsing statement_list\n"));
    
	TOKEN_TYPE expected[] = {WHILE_TYPE, IF_TYPE, CALL_TYPE, BEGIN_TYPE, ID_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, CALL_TYPE, EOF_TYPE, END_TYPE, ID_TYPE, WHILE_TYPE, IF_TYPE};
    
	switch(tok.type) {
        case WHILE_TYPE:
        case IF_TYPE:
        case CALL_TYPE:
        case BEGIN_TYPE:
        case ID_TYPE:
            // Grammar production: statement statement_list_2
            parse_statement();
            parse_statement_list_2();
            return;
        default:
            error();
            return;
	}
}

void parse_statement_list_2() {
	// first(statement_list_2): EPSILON, SEMICOLON
	// follow(statement_list_2): END
    
	DEBUG_PRINT(("Parsing statement_list_2\n"));
    
	TOKEN_TYPE expected[] = {END_TYPE, SEMICOLON_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case END_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case SEMICOLON_TYPE:
            // Grammar production: SEMICOLON statement statement_list_2
            match(SEMICOLON_TYPE);
            parse_statement();
            parse_statement_list_2();
            return;
        default:
            error();
            return;
	}
}

void parse_statement() {
	// first(statement): WHILE, CALL, IF, BEGIN, ID
	// follow(statement): END, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing statement\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, CALL_TYPE, ID_TYPE, WHILE_TYPE, IF_TYPE};
	TOKEN_TYPE synch_set[] = {WHILE_TYPE, CALL_TYPE, EOF_TYPE, SEMICOLON_TYPE, BEGIN_TYPE, END_TYPE, ELSE_TYPE, ID_TYPE, IF_TYPE};
    
	switch(tok.type) {
        case IF_TYPE:
            // Grammar production: IF expression THEN statement statement_2
            match(IF_TYPE);
            parse_expression();
            match(THEN_TYPE);
            parse_statement();
            parse_statement_2();
            return;
        case WHILE_TYPE:
            // Grammar production: WHILE expression DO statement
            match(WHILE_TYPE);
            parse_expression();
            match(DO_TYPE);
            parse_statement();
            return;
        case BEGIN_TYPE:
            // Grammar production: compound_statement
            parse_compound_statement();
            return;
        case CALL_TYPE:
            // Grammar production: procedure_statement
            parse_procedure_statement();
            return;
        case ID_TYPE:
            // Grammar production: variable ASSIGNOP expression
            parse_variable();
            match(ASSIGNOP_TYPE);
            parse_expression();
            return;
        default:
            error();
            return;
	}
}

void parse_statement_2() {
	// first(statement_2): EPSILON, ELSE
	// follow(statement_2): END, SEMICOLON, ELSE
	// MULTIPLE PRODUCTIONS FOR SYMBOL: ELSE
    
	DEBUG_PRINT(("Parsing statement_2\n"));
    
	TOKEN_TYPE expected[] = {END_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case ELSE_TYPE:
            // Grammar production: ELSE statement
            match(ELSE_TYPE);
            parse_statement();
            return;
        case END_TYPE:
        case SEMICOLON_TYPE:
        // case ELSE_TYPE: -- resolve dangling else
            // Grammar production: EPSILON
            // Epslion production
            return;
        default:
            error();
            return;
	}
}

void parse_variable() {
	// first(variable): ID
	// follow(variable): ASSIGNOP
    
	DEBUG_PRINT(("Parsing variable\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {ASSIGNOP_TYPE, EOF_TYPE, ID_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
            // Grammar production: ID variable_2
            match(ID_TYPE);
            parse_variable_2();
            return;
        default:
            error();
            return;
	}
}

void parse_variable_2() {
	// first(variable_2): LBRACKET, EPSILON
	// follow(variable_2): ASSIGNOP
    
	DEBUG_PRINT(("Parsing variable_2\n"));
    
	TOKEN_TYPE expected[] = {LBRACKET_TYPE, ASSIGNOP_TYPE};
	TOKEN_TYPE synch_set[] = {LBRACKET_TYPE, ASSIGNOP_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ASSIGNOP_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case LBRACKET_TYPE:
            // Grammar production: LBRACKET expression RBRACKET
            match(LBRACKET_TYPE);
            parse_expression();
            match(RBRACKET_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_procedure_statement() {
	// first(procedure_statement): CALL
	// follow(procedure_statement): END, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing procedure_statement\n"));
    
	TOKEN_TYPE expected[] = {CALL_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, CALL_TYPE, EOF_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case CALL_TYPE:
            // Grammar production: CALL ID procedure_statement_2
            match(CALL_TYPE);
            match(ID_TYPE);
            parse_procedure_statement_2();
            return;
        default:
            error();
            return;
	}
}

void parse_procedure_statement_2() {
	// first(procedure_statement_2): EPSILON, LPAREN
	// follow(procedure_statement_2): END, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing procedure_statement_2\n"));
    
	TOKEN_TYPE expected[] = {END_TYPE, SEMICOLON_TYPE, LPAREN_TYPE, ELSE_TYPE};
	TOKEN_TYPE synch_set[] = {SEMICOLON_TYPE, END_TYPE, EOF_TYPE, LPAREN_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case END_TYPE:
        case SEMICOLON_TYPE:
        case ELSE_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case LPAREN_TYPE:
            // Grammar production: LPAREN expression_list RPAREN
            match(LPAREN_TYPE);
            parse_expression_list();
            match(RPAREN_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_expression_list() {
	// first(expression_list): ID, NUM, PLUS, LPAREN, NOT, MINUS
	// follow(expression_list): RPAREN
    
	DEBUG_PRINT(("Parsing expression_list\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE, NUM_TYPE, PLUS_TYPE, LPAREN_TYPE, NOT_TYPE, MINUS_TYPE};
	TOKEN_TYPE synch_set[] = {NUM_TYPE, PLUS_TYPE, EOF_TYPE, LPAREN_TYPE, NOT_TYPE, RPAREN_TYPE, ID_TYPE, MINUS_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
        case NUM_TYPE:
        case PLUS_TYPE:
        case LPAREN_TYPE:
        case NOT_TYPE:
        case MINUS_TYPE:
            // Grammar production: expression expression_list_2
            parse_expression();
            parse_expression_list_2();
            return;
        default:
            error();
            return;
	}
}

void parse_expression_list_2() {
	// first(expression_list_2): EPSILON, COMMA
	// follow(expression_list_2): RPAREN
    
	DEBUG_PRINT(("Parsing expression_list_2\n"));
    
	TOKEN_TYPE expected[] = {RPAREN_TYPE, COMMA_TYPE};
	TOKEN_TYPE synch_set[] = {COMMA_TYPE, EOF_TYPE, RPAREN_TYPE};
    
	switch(tok.type) {
        case COMMA_TYPE:
            // Grammar production: COMMA expression expression_list_2
            match(COMMA_TYPE);
            parse_expression();
            parse_expression_list_2();
            return;
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        default:
            error();
            return;
	}
}

void parse_expression() {
	// first(expression): MINUS, NUM, PLUS, LPAREN, NOT, ID
	// follow(expression): DO, END, SEMICOLON, THEN, RPAREN, RBRACKET, COMMA, ELSE
    
	DEBUG_PRINT(("Parsing expression\n"));
    
	TOKEN_TYPE expected[] = {MINUS_TYPE, NUM_TYPE, PLUS_TYPE, LPAREN_TYPE, NOT_TYPE, ID_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, ID_TYPE, NUM_TYPE, PLUS_TYPE, LPAREN_TYPE, NOT_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE};
    
	switch(tok.type) {
        case MINUS_TYPE:
        case NUM_TYPE:
        case PLUS_TYPE:
        case LPAREN_TYPE:
        case NOT_TYPE:
        case ID_TYPE:
            // Grammar production: simple_expression expression_2
            parse_simple_expression();
            parse_expression_2();
            return;
        default:
            error();
            return;
	}
}

void parse_expression_2() {
	// first(expression_2): EPSILON, RELOP
	// follow(expression_2): DO, END, SEMICOLON, THEN, ELSE, COMMA, RPAREN, RBRACKET
    
	DEBUG_PRINT(("Parsing expression_2\n"));
    
	TOKEN_TYPE expected[] = {DO_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE, RELOP_TYPE};
    
	switch(tok.type) {
        case DO_TYPE:
        case END_TYPE:
        case SEMICOLON_TYPE:
        case THEN_TYPE:
        case ELSE_TYPE:
        case COMMA_TYPE:
        case RPAREN_TYPE:
        case RBRACKET_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case RELOP_TYPE:
            // Grammar production: RELOP simple_expression
            match(RELOP_TYPE);
            parse_simple_expression();
            return;
        default:
            error();
            return;
	}
}

void parse_simple_expression() {
	// first(simple_expression): NUM, PLUS, LPAREN, NOT, ID, MINUS
	// follow(simple_expression): DO, END, RELOP, SEMICOLON, THEN, ELSE, COMMA, RPAREN, RBRACKET
    
	DEBUG_PRINT(("Parsing simple_expression\n"));
    
	TOKEN_TYPE expected[] = {MINUS_TYPE, NUM_TYPE, PLUS_TYPE, LPAREN_TYPE, NOT_TYPE, ID_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, THEN_TYPE, ELSE_TYPE, LPAREN_TYPE, ID_TYPE, RELOP_TYPE, RPAREN_TYPE, SEMICOLON_TYPE, MINUS_TYPE, NUM_TYPE, PLUS_TYPE, NOT_TYPE, COMMA_TYPE, RBRACKET_TYPE};
    
	switch(tok.type) {
        case MINUS_TYPE:
        case PLUS_TYPE:
            // Grammar production: sign term simple_expression_2
            parse_sign();
            parse_term();
            parse_simple_expression_2();
            return;
        case NUM_TYPE:
        case LPAREN_TYPE:
        case NOT_TYPE:
        case ID_TYPE:
            // Grammar production: term simple_expression_2
            parse_term();
            parse_simple_expression_2();
            return;
        default:
            error();
            return;
	}
}

void parse_simple_expression_2() {
	// first(simple_expression_2): ADDOP, PLUS, MINUS, EPSILON
	// follow(simple_expression_2): DO, END, RELOP, SEMICOLON, THEN, ELSE, COMMA, RPAREN, RBRACKET
    
	DEBUG_PRINT(("Parsing simple_expression_2\n"));
    
	TOKEN_TYPE expected[] = {ADDOP_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, DO_TYPE, THEN_TYPE, ELSE_TYPE, PLUS_TYPE, COMMA_TYPE, RBRACKET_TYPE, MINUS_TYPE, RPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {ADDOP_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, DO_TYPE, THEN_TYPE, RPAREN_TYPE, ELSE_TYPE, PLUS_TYPE, COMMA_TYPE, RBRACKET_TYPE, MINUS_TYPE, RELOP_TYPE};
    
	switch(tok.type) {
        case ADDOP_TYPE:
            // Grammar production: ADDOP term simple_expression_2
            match(ADDOP_TYPE);
            parse_term();
            parse_simple_expression_2();
            return;
        case END_TYPE:
        case RELOP_TYPE:
        case SEMICOLON_TYPE:
        case DO_TYPE:
        case THEN_TYPE:
        case ELSE_TYPE:
        case COMMA_TYPE:
        case RBRACKET_TYPE:
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case MINUS_TYPE:
            // Grammar production: MINUS term simple_expression_2
            match(MINUS_TYPE);
            parse_term();
            parse_simple_expression_2();
            return;
        case PLUS_TYPE:
            // Grammar production: PLUS term simple_expression_2
            match(PLUS_TYPE);
            parse_term();
            parse_simple_expression_2();
            return;
        default:
            error();
            return;
	}
}

void parse_term() {
	// first(term): NUM, LPAREN, NOT, ID
	// follow(term): DO, END, RELOP, THEN, ELSE, MINUS, ADDOP, RPAREN, SEMICOLON, PLUS, COMMA, RBRACKET
    
	DEBUG_PRINT(("Parsing term\n"));
    
	TOKEN_TYPE expected[] = {NOT_TYPE, NUM_TYPE, ID_TYPE, LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, THEN_TYPE, ELSE_TYPE, LPAREN_TYPE, ID_TYPE, RELOP_TYPE, ADDOP_TYPE, RPAREN_TYPE, SEMICOLON_TYPE, MINUS_TYPE, NUM_TYPE, PLUS_TYPE, NOT_TYPE, COMMA_TYPE, RBRACKET_TYPE};
    
	switch(tok.type) {
        case NOT_TYPE:
        case NUM_TYPE:
        case ID_TYPE:
        case LPAREN_TYPE:
            // Grammar production: factor term_2
            parse_factor();
            parse_term_2();
            return;
        default:
            error();
            return;
	}
}

void parse_term_2() {
	// first(term_2): EPSILON, MULOP
	// follow(term_2): DO, END, RELOP, SEMICOLON, THEN, ELSE, ADDOP, PLUS, RPAREN, RBRACKET, COMMA, MINUS
    
	DEBUG_PRINT(("Parsing term_2\n"));
    
	TOKEN_TYPE expected[] = {DO_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, ADDOP_TYPE, PLUS_TYPE, MULOP_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, ADDOP_TYPE, PLUS_TYPE, MULOP_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE, RELOP_TYPE};
    
	switch(tok.type) {
        case DO_TYPE:
        case END_TYPE:
        case RELOP_TYPE:
        case SEMICOLON_TYPE:
        case THEN_TYPE:
        case ELSE_TYPE:
        case ADDOP_TYPE:
        case PLUS_TYPE:
        case RPAREN_TYPE:
        case RBRACKET_TYPE:
        case COMMA_TYPE:
        case MINUS_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case MULOP_TYPE:
            // Grammar production: MULOP factor term_2
            match(MULOP_TYPE);
            parse_factor();
            parse_term_2();
            return;
        default:
            error();
            return;
	}
}

void parse_factor() {
	// first(factor): NOT, NUM, ID, LPAREN
	// follow(factor): DO, END, RELOP, THEN, ELSE, MULOP, MINUS, ADDOP, RPAREN, SEMICOLON, PLUS, COMMA, RBRACKET
    
	DEBUG_PRINT(("Parsing factor\n"));
    
	TOKEN_TYPE expected[] = {NOT_TYPE, NUM_TYPE, ID_TYPE, LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, THEN_TYPE, ELSE_TYPE, LPAREN_TYPE, ID_TYPE, RELOP_TYPE, ADDOP_TYPE, RPAREN_TYPE, SEMICOLON_TYPE, MINUS_TYPE, NUM_TYPE, PLUS_TYPE, NOT_TYPE, COMMA_TYPE, RBRACKET_TYPE, MULOP_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
            // Grammar production: ID factor_2
            match(ID_TYPE);
            parse_factor_2();
            return;
        case LPAREN_TYPE:
            // Grammar production: LPAREN expression RPAREN
            match(LPAREN_TYPE);
            parse_expression();
            match(RPAREN_TYPE);
            return;
        case NOT_TYPE:
            // Grammar production: NOT factor
            match(NOT_TYPE);
            parse_factor();
            return;
        case NUM_TYPE:
            // Grammar production: NUM
            match(NUM_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_factor_2() {
	// first(factor_2): LBRACKET, EPSILON
	// follow(factor_2): DO, END, RELOP, SEMICOLON, THEN, ELSE, ADDOP, PLUS, MULOP, RPAREN, RBRACKET, COMMA, MINUS
    
	DEBUG_PRINT(("Parsing factor_2\n"));
    
	TOKEN_TYPE expected[] = {DO_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, LBRACKET_TYPE, ADDOP_TYPE, PLUS_TYPE, MULOP_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, THEN_TYPE, ELSE_TYPE, LBRACKET_TYPE, MULOP_TYPE, MINUS_TYPE, RELOP_TYPE, ADDOP_TYPE, RPAREN_TYPE, SEMICOLON_TYPE, PLUS_TYPE, COMMA_TYPE, RBRACKET_TYPE};
    
	switch(tok.type) {
        case DO_TYPE:
        case END_TYPE:
        case RELOP_TYPE:
        case SEMICOLON_TYPE:
        case THEN_TYPE:
        case ELSE_TYPE:
        case ADDOP_TYPE:
        case PLUS_TYPE:
        case MULOP_TYPE:
        case RPAREN_TYPE:
        case RBRACKET_TYPE:
        case COMMA_TYPE:
        case MINUS_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case LBRACKET_TYPE:
            // Grammar production: LBRACKET expression RBRACKET
            match(LBRACKET_TYPE);
            parse_expression();
            match(RBRACKET_TYPE);
            return;
        default:
            error();
            return;
	}
}

void parse_sign() {
	// first(sign): PLUS, MINUS
	// follow(sign): NOT, NUM, ID, LPAREN
    
	DEBUG_PRINT(("Parsing sign\n"));
    
	TOKEN_TYPE expected[] = {PLUS_TYPE, MINUS_TYPE};
	TOKEN_TYPE synch_set[] = {NUM_TYPE, PLUS_TYPE, EOF_TYPE, LPAREN_TYPE, NOT_TYPE, MINUS_TYPE, ID_TYPE};
    
	switch(tok.type) {
        case MINUS_TYPE:
            // Grammar production: MINUS
            match(MINUS_TYPE);
            return;
        case PLUS_TYPE:
            // Grammar production: PLUS
            match(PLUS_TYPE);
            return;
        default:
            error();
            return;
	}
}

