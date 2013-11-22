#include "compiler.h"
#include "parser.h"
#include "parsergen.h"

void parse_program() {
	// first(program): PROGRAM
	// follow(program): EOF
    
	DEBUG_PRINT(("Parsing program\n"));
    
	TOKEN_TYPE expected[] = {PROGRAM_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE};
    
	switch(tok.type) {
        case PROGRAM_TYPE:
            // Grammar production: PROGRAM ID LPAREN identifier_list RPAREN SEMICOLON program_2
            if (!match(PROGRAM_TYPE))
                goto synch;
            if (!match(ID_TYPE))
                goto synch;
            if (!match(LPAREN_TYPE))
                goto synch;
            parse_identifier_list();
            if (!match(RPAREN_TYPE))
                goto synch;
            if (!match(SEMICOLON_TYPE))
                goto synch;
            parse_program_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_program_2() {
	// first(program_2): VAR, BEGIN, PROCEDURE
	// follow(program_2): EOF
    
	DEBUG_PRINT(("Parsing program_2\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE, BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: compound_statement PERIOD
            parse_compound_statement();
            if (!match(PERIOD_TYPE))
                goto synch;
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
            if (!match(PERIOD_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_program_3() {
	// first(program_3): BEGIN, PROCEDURE
	// follow(program_3): EOF
    
	DEBUG_PRINT(("Parsing program_3\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: compound_statement PERIOD
            parse_compound_statement();
            if (!match(PERIOD_TYPE))
                goto synch;
            return;
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declarations compound_statement PERIOD
            parse_subprogram_declarations();
            parse_compound_statement();
            if (!match(PERIOD_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_identifier_list() {
	// first(identifier_list): ID
	// follow(identifier_list): COLON, RPAREN
    
	DEBUG_PRINT(("Parsing identifier_list\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, COLON_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
            // Grammar production: ID identifier_list_2
            if (!match(ID_TYPE))
                goto synch;
            parse_identifier_list_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_identifier_list_2() {
	// first(identifier_list_2): EPSILON, COMMA
	// follow(identifier_list_2): COLON, RPAREN
    
	DEBUG_PRINT(("Parsing identifier_list_2\n"));
    
	TOKEN_TYPE expected[] = {COLON_TYPE, COMMA_TYPE, RPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, COLON_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case COMMA_TYPE:
            // Grammar production: COMMA ID identifier_list_2
            if (!match(COMMA_TYPE))
                goto synch;
            if (!match(ID_TYPE))
                goto synch;
            parse_identifier_list_2();
            return;
        case COLON_TYPE:
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_declarations() {
	// first(declarations): VAR
	// follow(declarations): BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing declarations\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case VAR_TYPE:
            // Grammar production: VAR identifier_list COLON type SEMICOLON declarations_2
            if (!match(VAR_TYPE))
                goto synch;
            parse_identifier_list();
            if (!match(COLON_TYPE))
                goto synch;
            parse_type();
            if (!match(SEMICOLON_TYPE))
                goto synch;
            parse_declarations_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_declarations_2() {
	// first(declarations_2): VAR, EPSILON
	// follow(declarations_2): BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing declarations_2\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE, BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
        case PROCEDURE_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case VAR_TYPE:
            // Grammar production: VAR identifier_list COLON type SEMICOLON declarations_2
            if (!match(VAR_TYPE))
                goto synch;
            parse_identifier_list();
            if (!match(COLON_TYPE))
                goto synch;
            parse_type();
            if (!match(SEMICOLON_TYPE))
                goto synch;
            parse_declarations_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_type() {
	// first(type): REAL, INTEGER, ARRAY
	// follow(type): RPAREN, SEMICOLON
    
	DEBUG_PRINT(("Parsing type\n"));
    
	TOKEN_TYPE expected[] = {REAL_TYPE, INTEGER_TYPE, ARRAY_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case ARRAY_TYPE:
            // Grammar production: ARRAY LBRACKET NUM ARRAY_RANGE NUM RBRACKET OF standard_type
            if (!match(ARRAY_TYPE))
                goto synch;
            if (!match(LBRACKET_TYPE))
                goto synch;
            if (!match(NUM_TYPE))
                goto synch;
            if (!match(ARRAY_RANGE_TYPE))
                goto synch;
            if (!match(NUM_TYPE))
                goto synch;
            if (!match(RBRACKET_TYPE))
                goto synch;
            if (!match(OF_TYPE))
                goto synch;
            parse_standard_type();
            return;
        case REAL_TYPE:
        case INTEGER_TYPE:
            // Grammar production: standard_type
            parse_standard_type();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_standard_type() {
	// first(standard_type): REAL, INTEGER
	// follow(standard_type): RPAREN, SEMICOLON
    
	DEBUG_PRINT(("Parsing standard_type\n"));
    
	TOKEN_TYPE expected[] = {REAL_TYPE, INTEGER_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case INTEGER_TYPE:
            // Grammar production: INTEGER
            if (!match(INTEGER_TYPE))
                goto synch;
            return;
        case REAL_TYPE:
            // Grammar production: REAL
            if (!match(REAL_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_subprogram_declarations() {
	// first(subprogram_declarations): PROCEDURE
	// follow(subprogram_declarations): BEGIN
    
	DEBUG_PRINT(("Parsing subprogram_declarations\n"));
    
	TOKEN_TYPE expected[] = {PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declaration SEMICOLON subprogram_declarations_2
            parse_subprogram_declaration();
            if (!match(SEMICOLON_TYPE))
                goto synch;
            parse_subprogram_declarations_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_subprogram_declarations_2() {
	// first(subprogram_declarations_2): EPSILON, PROCEDURE
	// follow(subprogram_declarations_2): BEGIN
    
	DEBUG_PRINT(("Parsing subprogram_declarations_2\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_declaration SEMICOLON subprogram_declarations_2
            parse_subprogram_declaration();
            if (!match(SEMICOLON_TYPE))
                goto synch;
            parse_subprogram_declarations_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_subprogram_declaration() {
	// first(subprogram_declaration): PROCEDURE
	// follow(subprogram_declaration): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration\n"));
    
	TOKEN_TYPE expected[] = {PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case PROCEDURE_TYPE:
            // Grammar production: subprogram_head subprogram_declaration_2
            parse_subprogram_head();
            parse_subprogram_declaration_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_subprogram_declaration_2() {
	// first(subprogram_declaration_2): VAR, BEGIN, PROCEDURE
	// follow(subprogram_declaration_2): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration_2\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE, BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE, SEMICOLON_TYPE};
    
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
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_subprogram_declaration_3() {
	// first(subprogram_declaration_3): BEGIN, PROCEDURE
	// follow(subprogram_declaration_3): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration_3\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE, SEMICOLON_TYPE};
    
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
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
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
            if (!match(PROCEDURE_TYPE))
                goto synch;
            if (!match(ID_TYPE))
                goto synch;
            parse_subprogram_head_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_subprogram_head_2() {
	// first(subprogram_head_2): LPAREN, SEMICOLON
	// follow(subprogram_head_2): VAR, BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing subprogram_head_2\n"));
    
	TOKEN_TYPE expected[] = {SEMICOLON_TYPE, LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {VAR_TYPE, BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case SEMICOLON_TYPE:
            // Grammar production: SEMICOLON
            if (!match(SEMICOLON_TYPE))
                goto synch;
            return;
        case LPAREN_TYPE:
            // Grammar production: arguments SEMICOLON
            parse_arguments();
            if (!match(SEMICOLON_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_arguments() {
	// first(arguments): LPAREN
	// follow(arguments): SEMICOLON
    
	DEBUG_PRINT(("Parsing arguments\n"));
    
	TOKEN_TYPE expected[] = {LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case LPAREN_TYPE:
            // Grammar production: LPAREN parameter_list RPAREN
            if (!match(LPAREN_TYPE))
                goto synch;
            parse_parameter_list();
            if (!match(RPAREN_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_parameter_list() {
	// first(parameter_list): ID
	// follow(parameter_list): RPAREN
    
	DEBUG_PRINT(("Parsing parameter_list\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
            // Grammar production: identifier_list COLON type parameter_list_2
            parse_identifier_list();
            if (!match(COLON_TYPE))
                goto synch;
            parse_type();
            parse_parameter_list_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_parameter_list_2() {
	// first(parameter_list_2): EPSILON, SEMICOLON
	// follow(parameter_list_2): RPAREN
    
	DEBUG_PRINT(("Parsing parameter_list_2\n"));
    
	TOKEN_TYPE expected[] = {RPAREN_TYPE, SEMICOLON_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case SEMICOLON_TYPE:
            // Grammar production: SEMICOLON identifier_list COLON type parameter_list_2
            if (!match(SEMICOLON_TYPE))
                goto synch;
            parse_identifier_list();
            if (!match(COLON_TYPE))
                goto synch;
            parse_type();
            parse_parameter_list_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_compound_statement() {
	// first(compound_statement): BEGIN
	// follow(compound_statement): END, PERIOD, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing compound_statement\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE};
	TOKEN_TYPE synch_set[] = {ELSE_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, PERIOD_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: BEGIN compound_statement_2
            if (!match(BEGIN_TYPE))
                goto synch;
            parse_compound_statement_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_compound_statement_2() {
	// first(compound_statement_2): BEGIN, END, CALL, IF, WHILE, ID
	// follow(compound_statement_2): END, PERIOD, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing compound_statement_2\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, END_TYPE, WHILE_TYPE, CALL_TYPE, ID_TYPE, IF_TYPE};
	TOKEN_TYPE synch_set[] = {ELSE_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, PERIOD_TYPE};
    
	switch(tok.type) {
        case END_TYPE:
            // Grammar production: END
            if (!match(END_TYPE))
                goto synch;
            return;
        case BEGIN_TYPE:
        case WHILE_TYPE:
        case CALL_TYPE:
        case ID_TYPE:
        case IF_TYPE:
            // Grammar production: optional_statements END
            parse_optional_statements();
            if (!match(END_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_optional_statements() {
	// first(optional_statements): BEGIN, WHILE, CALL, ID, IF
	// follow(optional_statements): END
    
	DEBUG_PRINT(("Parsing optional_statements\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, CALL_TYPE, WHILE_TYPE, ID_TYPE, IF_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE};
    
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
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_statement_list() {
	// first(statement_list): BEGIN, WHILE, CALL, ID, IF
	// follow(statement_list): END
    
	DEBUG_PRINT(("Parsing statement_list\n"));
    
	TOKEN_TYPE expected[] = {WHILE_TYPE, IF_TYPE, CALL_TYPE, BEGIN_TYPE, ID_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE};
    
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
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_statement_list_2() {
	// first(statement_list_2): EPSILON, SEMICOLON
	// follow(statement_list_2): END
    
	DEBUG_PRINT(("Parsing statement_list_2\n"));
    
	TOKEN_TYPE expected[] = {END_TYPE, SEMICOLON_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case END_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case SEMICOLON_TYPE:
            // Grammar production: SEMICOLON statement statement_list_2
            if (!match(SEMICOLON_TYPE))
                goto synch;
            parse_statement();
            parse_statement_list_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_statement() {
	// first(statement): WHILE, CALL, IF, BEGIN, ID
	// follow(statement): END, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing statement\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, CALL_TYPE, ID_TYPE, WHILE_TYPE, IF_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case IF_TYPE:
            // Grammar production: IF expression THEN statement statement_2
            if (!match(IF_TYPE))
                goto synch;
            parse_expression();
            if (!match(THEN_TYPE))
                goto synch;
            parse_statement();
            parse_statement_2();
            return;
        case WHILE_TYPE:
            // Grammar production: WHILE expression DO statement
            if (!match(WHILE_TYPE))
                goto synch;
            parse_expression();
            if (!match(DO_TYPE))
                goto synch;
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
            if (!match(ASSIGNOP_TYPE))
                goto synch;
            parse_expression();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
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
            if (!match(ELSE_TYPE))
                goto synch;
            parse_statement();
            return;
        case END_TYPE:
        case SEMICOLON_TYPE:
        // case ELSE_TYPE: -- resolve dangling else
            // Grammar production: EPSILON
            // Epslion production
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_variable() {
	// first(variable): ID
	// follow(variable): ASSIGNOP
    
	DEBUG_PRINT(("Parsing variable\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {ASSIGNOP_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
            // Grammar production: ID variable_2
            if (!match(ID_TYPE))
                goto synch;
            parse_variable_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_variable_2() {
	// first(variable_2): LBRACKET, EPSILON
	// follow(variable_2): ASSIGNOP
    
	DEBUG_PRINT(("Parsing variable_2\n"));
    
	TOKEN_TYPE expected[] = {LBRACKET_TYPE, ASSIGNOP_TYPE};
	TOKEN_TYPE synch_set[] = {ASSIGNOP_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ASSIGNOP_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case LBRACKET_TYPE:
            // Grammar production: LBRACKET expression RBRACKET
            if (!match(LBRACKET_TYPE))
                goto synch;
            parse_expression();
            if (!match(RBRACKET_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_procedure_statement() {
	// first(procedure_statement): CALL
	// follow(procedure_statement): END, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing procedure_statement\n"));
    
	TOKEN_TYPE expected[] = {CALL_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case CALL_TYPE:
            // Grammar production: CALL ID procedure_statement_2
            if (!match(CALL_TYPE))
                goto synch;
            if (!match(ID_TYPE))
                goto synch;
            parse_procedure_statement_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_procedure_statement_2() {
	// first(procedure_statement_2): EPSILON, LPAREN
	// follow(procedure_statement_2): END, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing procedure_statement_2\n"));
    
	TOKEN_TYPE expected[] = {END_TYPE, SEMICOLON_TYPE, LPAREN_TYPE, ELSE_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case END_TYPE:
        case SEMICOLON_TYPE:
        case ELSE_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        case LPAREN_TYPE:
            // Grammar production: LPAREN expression_list RPAREN
            if (!match(LPAREN_TYPE))
                goto synch;
            parse_expression_list();
            if (!match(RPAREN_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_expression_list() {
	// first(expression_list): NUM, PLUS, LPAREN, NOT, MINUS, ID
	// follow(expression_list): RPAREN
    
	DEBUG_PRINT(("Parsing expression_list\n"));
    
	TOKEN_TYPE expected[] = {MINUS_TYPE, NUM_TYPE, PLUS_TYPE, LPAREN_TYPE, NOT_TYPE, ID_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case MINUS_TYPE:
        case NUM_TYPE:
        case PLUS_TYPE:
        case LPAREN_TYPE:
        case NOT_TYPE:
        case ID_TYPE:
            // Grammar production: expression expression_list_2
            parse_expression();
            parse_expression_list_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_expression_list_2() {
	// first(expression_list_2): EPSILON, COMMA
	// follow(expression_list_2): RPAREN
    
	DEBUG_PRINT(("Parsing expression_list_2\n"));
    
	TOKEN_TYPE expected[] = {RPAREN_TYPE, COMMA_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case COMMA_TYPE:
            // Grammar production: COMMA expression expression_list_2
            if (!match(COMMA_TYPE))
                goto synch;
            parse_expression();
            parse_expression_list_2();
            return;
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_expression() {
	// first(expression): MINUS, NUM, PLUS, LPAREN, NOT, ID
	// follow(expression): DO, END, SEMICOLON, THEN, RPAREN, RBRACKET, COMMA, ELSE
    
	DEBUG_PRINT(("Parsing expression\n"));
    
	TOKEN_TYPE expected[] = {MINUS_TYPE, NUM_TYPE, PLUS_TYPE, LPAREN_TYPE, NOT_TYPE, ID_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE};
    
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
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_expression_2() {
	// first(expression_2): EPSILON, RELOP
	// follow(expression_2): DO, END, SEMICOLON, THEN, ELSE, COMMA, RPAREN, RBRACKET
    
	DEBUG_PRINT(("Parsing expression_2\n"));
    
	TOKEN_TYPE expected[] = {DO_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE};
    
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
            if (!match(RELOP_TYPE))
                goto synch;
            parse_simple_expression();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_simple_expression() {
	// first(simple_expression): NUM, PLUS, LPAREN, NOT, ID, MINUS
	// follow(simple_expression): DO, END, RELOP, SEMICOLON, THEN, ELSE, COMMA, RPAREN, RBRACKET
    
	DEBUG_PRINT(("Parsing simple_expression\n"));
    
	TOKEN_TYPE expected[] = {MINUS_TYPE, NUM_TYPE, PLUS_TYPE, LPAREN_TYPE, NOT_TYPE, ID_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE, RELOP_TYPE};
    
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
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_simple_expression_2() {
	// first(simple_expression_2): ADDOP, PLUS, MINUS, EPSILON
	// follow(simple_expression_2): DO, END, RELOP, SEMICOLON, THEN, ELSE, COMMA, RPAREN, RBRACKET
    
	DEBUG_PRINT(("Parsing simple_expression_2\n"));
    
	TOKEN_TYPE expected[] = {ADDOP_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, DO_TYPE, THEN_TYPE, ELSE_TYPE, PLUS_TYPE, COMMA_TYPE, RBRACKET_TYPE, MINUS_TYPE, RPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE, RELOP_TYPE};
    
	switch(tok.type) {
        case ADDOP_TYPE:
            // Grammar production: ADDOP term simple_expression_2
            if (!match(ADDOP_TYPE))
                goto synch;
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
            if (!match(MINUS_TYPE))
                goto synch;
            parse_term();
            parse_simple_expression_2();
            return;
        case PLUS_TYPE:
            // Grammar production: PLUS term simple_expression_2
            if (!match(PLUS_TYPE))
                goto synch;
            parse_term();
            parse_simple_expression_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_term() {
	// first(term): NUM, LPAREN, NOT, ID
	// follow(term): DO, END, RELOP, THEN, ELSE, MINUS, ADDOP, RPAREN, SEMICOLON, PLUS, COMMA, RBRACKET
    
	DEBUG_PRINT(("Parsing term\n"));
    
	TOKEN_TYPE expected[] = {NOT_TYPE, NUM_TYPE, ID_TYPE, LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, ADDOP_TYPE, PLUS_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE, RELOP_TYPE};
    
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
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_term_2() {
	// first(term_2): EPSILON, MULOP
	// follow(term_2): DO, END, RELOP, SEMICOLON, THEN, ELSE, ADDOP, PLUS, RPAREN, RBRACKET, COMMA, MINUS
    
	DEBUG_PRINT(("Parsing term_2\n"));
    
	TOKEN_TYPE expected[] = {DO_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, ADDOP_TYPE, PLUS_TYPE, MULOP_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, ADDOP_TYPE, PLUS_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE, RELOP_TYPE};
    
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
            if (!match(MULOP_TYPE))
                goto synch;
            parse_factor();
            parse_term_2();
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_factor() {
	// first(factor): NOT, NUM, ID, LPAREN
	// follow(factor): DO, END, RELOP, THEN, ELSE, MULOP, MINUS, ADDOP, RPAREN, SEMICOLON, PLUS, COMMA, RBRACKET
    
	DEBUG_PRINT(("Parsing factor\n"));
    
	TOKEN_TYPE expected[] = {NOT_TYPE, NUM_TYPE, ID_TYPE, LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, ADDOP_TYPE, PLUS_TYPE, MULOP_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE, RELOP_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
            // Grammar production: ID factor_2
            if (!match(ID_TYPE))
                goto synch;
            parse_factor_2();
            return;
        case LPAREN_TYPE:
            // Grammar production: LPAREN expression RPAREN
            if (!match(LPAREN_TYPE))
                goto synch;
            parse_expression();
            if (!match(RPAREN_TYPE))
                goto synch;
            return;
        case NOT_TYPE:
            // Grammar production: NOT factor
            if (!match(NOT_TYPE))
                goto synch;
            parse_factor();
            return;
        case NUM_TYPE:
            // Grammar production: NUM
            if (!match(NUM_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_factor_2() {
	// first(factor_2): LBRACKET, EPSILON
	// follow(factor_2): DO, END, RELOP, SEMICOLON, THEN, ELSE, ADDOP, PLUS, MULOP, RPAREN, RBRACKET, COMMA, MINUS
    
	DEBUG_PRINT(("Parsing factor_2\n"));
    
	TOKEN_TYPE expected[] = {DO_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, LBRACKET_TYPE, ADDOP_TYPE, PLUS_TYPE, MULOP_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE};
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
            if (!match(LBRACKET_TYPE))
                goto synch;
            parse_expression();
            if (!match(RBRACKET_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}

void parse_sign() {
	// first(sign): PLUS, MINUS
	// follow(sign): NOT, NUM, ID, LPAREN
    
	DEBUG_PRINT(("Parsing sign\n"));
    
	TOKEN_TYPE expected[] = {PLUS_TYPE, MINUS_TYPE};
	TOKEN_TYPE synch_set[] = {NOT_TYPE, NUM_TYPE, EOF_TYPE, LPAREN_TYPE, ID_TYPE};
    
	switch(tok.type) {
        case MINUS_TYPE:
            // Grammar production: MINUS
            if (!match(MINUS_TYPE))
                goto synch;
            return;
        case PLUS_TYPE:
            // Grammar production: PLUS
            if (!match(PLUS_TYPE))
                goto synch;
            return;
        default:
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
}
