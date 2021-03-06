#include <assert.h>
#include <stdlib.h>

#include "compiler.h"
#include "parser.h"
#include "parsergen.h"
#include "output.h"

type parse_program() {
	// first(program): PROGRAM
	// follow(program): EOF
    
	DEBUG_PRINT(("Parsing program\n"));
    
	TOKEN_TYPE expected[] = {PROGRAM_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE};
    
	switch(tok.type) {
        case PROGRAM_TYPE:
		{
            // Grammar production: PROGRAM ID LPAREN identifier_list RPAREN SEMICOLON program_2
            if (!match(PROGRAM_TYPE))
                goto synch;
            token id_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            stack_node *procedure_node = check_add_id(id_tok.lexeme, PROCEDURE, false, 0);
            // check_add_id() should never fail here, procedure_node should not be NULL
            assert(procedure_node);
            if (!match(LPAREN_TYPE))
                goto synch;
            type identifier_list_type = parse_identifier_list(procedure_node);
            if (!match(RPAREN_TYPE))
                goto synch;
            if (!match(SEMICOLON_TYPE))
                goto synch;
            type program_2_type = parse_program_2();
            pop_procedure(procedure_node);
            // TODO: renable when subprocedures are popped too
            assert(stack == procedure_node);
            if (identifier_list_type == ERROR_STAR || identifier_list_type == ERROR || program_2_type == ERROR_STAR || program_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "program", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_program_2() {
	// first(program_2): VAR, BEGIN, PROCEDURE
	// follow(program_2): EOF
    
	DEBUG_PRINT(("Parsing program_2\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE, BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
		{
            // Grammar production: compound_statement PERIOD
            type compound_statement_type = parse_compound_statement();
            if (!match(PERIOD_TYPE))
                goto synch;
            if (compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        case VAR_TYPE:
		{
            // Grammar production: declarations program_3
            type declarations_type = parse_declarations();
            type program_3_type = parse_program_3();
            if (declarations_type == ERROR_STAR || declarations_type == ERROR || program_3_type == ERROR_STAR || program_3_type == ERROR)
                return ERROR;
            return NONE;
		}
        case PROCEDURE_TYPE:
		{
            // Grammar production: subprogram_declarations compound_statement PERIOD
            type subprogram_declarations_type = parse_subprogram_declarations();
            type compound_statement_type = parse_compound_statement();
            if (!match(PERIOD_TYPE))
                goto synch;
            if (subprogram_declarations_type == ERROR_STAR || subprogram_declarations_type == ERROR || compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "program_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_program_3() {
	// first(program_3): BEGIN, PROCEDURE
	// follow(program_3): EOF
    
	DEBUG_PRINT(("Parsing program_3\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
		{
            // Grammar production: compound_statement PERIOD
            type compound_statement_type = parse_compound_statement();
            if (!match(PERIOD_TYPE))
                goto synch;
            if (compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        case PROCEDURE_TYPE:
		{
            // Grammar production: subprogram_declarations compound_statement PERIOD
            type subprogram_declarations_type = parse_subprogram_declarations();
            type compound_statement_type = parse_compound_statement();
            if (!match(PERIOD_TYPE))
                goto synch;
            if (subprogram_declarations_type == ERROR_STAR || subprogram_declarations_type == ERROR || compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "program_3", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_identifier_list(stack_node *procedure_node) {
	// first(identifier_list): ID
	// follow(identifier_list): COLON, RPAREN
    
	DEBUG_PRINT(("Parsing identifier_list\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, COLON_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
		{
            // Grammar production: ID identifier_list_2
            token id_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            stack_node *param_node = check_add_parameter(procedure_node, id_tok.lexeme);
            if (!param_node) {
                stack_node *param_fix_node = check_add_parameter(procedure_node, NULL);
                // Ignore return value of parse_identifier_list_2(), return error regardless
                // Error written to listing file by check_add_parameter
                parse_identifier_list_2(param_fix_node);
                return ERROR_STAR;
            }
            type identifier_list_2_type = parse_identifier_list_2(param_node);
            if (identifier_list_2_type == ERROR_STAR || identifier_list_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "identifier_list", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_identifier_list_2(stack_node *prev_param_node) {
	// first(identifier_list_2): EPSILON, COMMA
	// follow(identifier_list_2): COLON, RPAREN
    
	DEBUG_PRINT(("Parsing identifier_list_2\n"));
    
	TOKEN_TYPE expected[] = {COLON_TYPE, COMMA_TYPE, RPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, COLON_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case COMMA_TYPE:
		{
            // Grammar production: COMMA ID identifier_list_2
            if (!match(COMMA_TYPE))
                goto synch;
            token id_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            stack_node *param_node = check_add_parameter(prev_param_node, id_tok.lexeme);
            if (!param_node) {
                stack_node *param_fix_node = check_add_parameter(prev_param_node, NULL);
                // Ignore return value of parse_identifier_list_2(), return error regardless
                // Error written to listing file by check_add_parameter
                parse_identifier_list_2(param_fix_node);
                return ERROR_STAR;
            }
            type identifier_list_2_type = parse_identifier_list_2(param_node);
            if (identifier_list_2_type == ERROR_STAR || identifier_list_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        case COLON_TYPE:
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return NONE;
        default:
            write_listing_synerr(lineno, tok, "identifier_list_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_declarations() {
	// first(declarations): VAR
	// follow(declarations): BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing declarations\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case VAR_TYPE:
		{
            // Grammar production: VAR ID COLON type SEMICOLON declarations_2
            if (!match(VAR_TYPE))
                goto synch;
            token id_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            if (!match(COLON_TYPE))
                goto synch;
            int array_size;
            type type_type = parse_type(&array_size);
            type var_type = type_type;
            if (var_type == ERROR_STAR) {
                var_type = ERROR;
            }
            stack_node *var_node = check_add_id(id_tok.lexeme, var_type, true, array_size);
            if (!match(SEMICOLON_TYPE))
                goto synch;
            type declarations_2_type = parse_declarations_2();
            if (!var_node)
                // Error written to listing file by check_add_id
                return ERROR_STAR;
            if (type_type == ERROR_STAR || type_type == ERROR || declarations_2_type == ERROR_STAR || declarations_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "declarations", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_declarations_2() {
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
            return NONE;
        case VAR_TYPE:
		{
            // Grammar production: VAR ID COLON type SEMICOLON declarations_2
            if (!match(VAR_TYPE))
                goto synch;
            token id_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            if (!match(COLON_TYPE))
                goto synch;
            int array_size = 0;
            type type_type = parse_type(&array_size);
            type var_type = type_type;
            if (var_type == ERROR_STAR) {
                var_type = ERROR;
            }
            stack_node *var_node = check_add_id(id_tok.lexeme, var_type, true, array_size);
            if (!match(SEMICOLON_TYPE))
                goto synch;
            type declarations_2_type = parse_declarations_2();
            if (!var_node)
                // Error written to listing file by check_add_id
                return ERROR_STAR;
            if (type_type == ERROR_STAR || type_type == ERROR || declarations_2_type == ERROR_STAR || declarations_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "declarations_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_type(int *size) {
	// first(type): REAL, INTEGER, ARRAY
	// follow(type): RPAREN, SEMICOLON
    
	DEBUG_PRINT(("Parsing type\n"));
    
	TOKEN_TYPE expected[] = {REAL_TYPE, INTEGER_TYPE, ARRAY_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case ARRAY_TYPE:
		{
            // Grammar production: ARRAY LBRACKET NUM ARRAY_RANGE NUM RBRACKET OF standard_type
            bool error = false;
            if (!match(ARRAY_TYPE))
                goto synch;
            if (!match(LBRACKET_TYPE))
                goto synch;
            token num1_tok = tok;
            if (!match(NUM_TYPE))
                goto synch;
            if (num1_tok.attr.num != INTEGER_NUM) {
                error = true;
                write_listing_symerr("Number \"%s\" must be an integer", num1_tok.lexeme);
            }
            if (!match(ARRAY_RANGE_TYPE))
                goto synch;
            token num2_tok = tok;
            if (!match(NUM_TYPE))
                goto synch;
            if (num2_tok.attr.num != INTEGER_NUM) {
                error = true;
                write_listing_symerr("Number \"%s\" must be an integer", num2_tok.lexeme);
            }
            if (!match(RBRACKET_TYPE))
                goto synch;
            if (!match(OF_TYPE))
                goto synch;
            type standard_type_type = parse_standard_type();
            if (standard_type_type == ERROR_STAR || standard_type_type == ERROR)
                return ERROR;
            if (error)
                return ERROR_STAR;

            *size = atoi(num2_tok.lexeme) - atoi(num1_tok.lexeme);
            
            if (standard_type_type == INTEGER)
                return ARRAY_INTEGER;
            if (standard_type_type == REAL)
                return ARRAY_REAL;
            // parse_standard_type() should never return anything other than INTEGER, REAL, or ERROR_STAR
            assert(false);
            return ERROR_STAR;
		}
        case REAL_TYPE:
        case INTEGER_TYPE:
		{
            // Grammar production: standard_type
            type standard_type_type = parse_standard_type();
            if (standard_type_type == ERROR_STAR || standard_type_type == ERROR)
                return ERROR;
            return standard_type_type;
		}
        default:
            write_listing_synerr(lineno, tok, "type", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_standard_type() {
	// first(standard_type): REAL, INTEGER
	// follow(standard_type): RPAREN, SEMICOLON
    
	DEBUG_PRINT(("Parsing standard_type\n"));
    
	TOKEN_TYPE expected[] = {REAL_TYPE, INTEGER_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case INTEGER_TYPE:
		{
            // Grammar production: INTEGER
            if (!match(INTEGER_TYPE))
                goto synch;
            return INTEGER;
		}
        case REAL_TYPE:
		{
            // Grammar production: REAL
            if (!match(REAL_TYPE))
                goto synch;
            return REAL;
		}
        default:
            write_listing_synerr(lineno, tok, "standard_type", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_subprogram_declarations() {
	// first(subprogram_declarations): PROCEDURE
	// follow(subprogram_declarations): BEGIN
    
	DEBUG_PRINT(("Parsing subprogram_declarations\n"));
    
	TOKEN_TYPE expected[] = {PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case PROCEDURE_TYPE:
		{
            // Grammar production: subprogram_declaration SEMICOLON subprogram_declarations_2
            type subprogram_declaration_type = parse_subprogram_declaration();
            if (!match(SEMICOLON_TYPE))
                goto synch;
            type subprogram_declarations_2_type = parse_subprogram_declarations_2();
            if (subprogram_declaration_type == ERROR_STAR || subprogram_declaration_type == ERROR || subprogram_declarations_2_type == ERROR_STAR || subprogram_declarations_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "subprogram_declarations", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_subprogram_declarations_2() {
	// first(subprogram_declarations_2): EPSILON, PROCEDURE
	// follow(subprogram_declarations_2): BEGIN
    
	DEBUG_PRINT(("Parsing subprogram_declarations_2\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {BEGIN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return NONE;
        case PROCEDURE_TYPE:
		{
            // Grammar production: subprogram_declaration SEMICOLON subprogram_declarations_2
            type subprogram_declaration_type = parse_subprogram_declaration();
            if (!match(SEMICOLON_TYPE))
                goto synch;
            type subprogram_declarations_2_type = parse_subprogram_declarations_2();
            if (subprogram_declaration_type == ERROR_STAR || subprogram_declaration_type == ERROR || subprogram_declarations_2_type == ERROR_STAR || subprogram_declarations_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "subprogram_declarations_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_subprogram_declaration() {
	// first(subprogram_declaration): PROCEDURE
	// follow(subprogram_declaration): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration\n"));
    
	TOKEN_TYPE expected[] = {PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case PROCEDURE_TYPE:
		{
            // Grammar production: subprogram_head subprogram_declaration_2
            stack_node *subprogram_node = NULL;
            type subprogram_head_type = parse_subprogram_head(&subprogram_node);
            type subprogram_declaration_2_type = parse_subprogram_declaration_2();
            // subprogram_node may be NULL if syntax error in subprogram_head
            if (subprogram_node) {
                pop_procedure(subprogram_node);
            }
            if (subprogram_head_type == ERROR_STAR || subprogram_head_type == ERROR || subprogram_declaration_2_type == ERROR_STAR || subprogram_declaration_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "subprogram_declaration", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_subprogram_declaration_2() {
	// first(subprogram_declaration_2): VAR, BEGIN, PROCEDURE
	// follow(subprogram_declaration_2): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration_2\n"));
    
	TOKEN_TYPE expected[] = {VAR_TYPE, BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
		{
            // Grammar production: compound_statement
            type compound_statement_type = parse_compound_statement();
            if (compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        case VAR_TYPE:
		{
            // Grammar production: declarations subprogram_declaration_3
            type declarations_type = parse_declarations();
            type subprogram_declaration_3_type = parse_subprogram_declaration_3();
            if (declarations_type == ERROR_STAR || declarations_type == ERROR || subprogram_declaration_3_type == ERROR_STAR || subprogram_declaration_3_type == ERROR)
                return ERROR;
            return NONE;
		}
        case PROCEDURE_TYPE:
		{
            // Grammar production: subprogram_declarations compound_statement
            type subprogram_declarations_type = parse_subprogram_declarations();
            type compound_statement_type = parse_compound_statement();
            if (subprogram_declarations_type == ERROR_STAR || subprogram_declarations_type == ERROR || compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "subprogram_declaration_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_subprogram_declaration_3() {
	// first(subprogram_declaration_3): BEGIN, PROCEDURE
	// follow(subprogram_declaration_3): SEMICOLON
    
	DEBUG_PRINT(("Parsing subprogram_declaration_3\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
		{
            // Grammar production: compound_statement
            type compound_statement_type = parse_compound_statement();
            if (compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        case PROCEDURE_TYPE:
		{
            // Grammar production: subprogram_declarations compound_statement
            type subprogram_declarations_type = parse_subprogram_declarations();
            type compound_statement_type = parse_compound_statement();
            if (subprogram_declarations_type == ERROR_STAR || subprogram_declarations_type == ERROR || compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "subprogram_declaration_3", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_subprogram_head(stack_node **subprogram_node) {
	// first(subprogram_head): PROCEDURE
	// follow(subprogram_head): VAR, BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing subprogram_head\n"));
    
	TOKEN_TYPE expected[] = {PROCEDURE_TYPE};
	TOKEN_TYPE synch_set[] = {VAR_TYPE, BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case PROCEDURE_TYPE:
		{
            // Grammar production: PROCEDURE ID subprogram_head_2
            if (!match(PROCEDURE_TYPE))
                goto synch;
            token id_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            stack_node *procedure_node = check_add_id(id_tok.lexeme, PROCEDURE, false, 0);
            *subprogram_node = procedure_node;
            // Should always sucessfully create node if not limiting scope
            assert(procedure_node);
            type subprogram_head_2_type = parse_subprogram_head_2(procedure_node);
            if (subprogram_head_2_type == ERROR_STAR || subprogram_head_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "subprogram_head", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_subprogram_head_2(stack_node *procedure_node) {
	// first(subprogram_head_2): LPAREN, SEMICOLON
	// follow(subprogram_head_2): VAR, BEGIN, PROCEDURE
    
	DEBUG_PRINT(("Parsing subprogram_head_2\n"));
    
	TOKEN_TYPE expected[] = {SEMICOLON_TYPE, LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {VAR_TYPE, BEGIN_TYPE, EOF_TYPE, PROCEDURE_TYPE};
    
	switch(tok.type) {
        case SEMICOLON_TYPE:
		{
            // Grammar production: SEMICOLON
            if (!match(SEMICOLON_TYPE))
                goto synch;
            return NONE;
		}
        case LPAREN_TYPE:
		{
            // Grammar production: arguments SEMICOLON
            type arguments_type = parse_arguments(procedure_node);
            if (!match(SEMICOLON_TYPE))
                goto synch;
            if (arguments_type == ERROR_STAR || arguments_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "subprogram_head_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_arguments(stack_node *procedure_node) {
	// first(arguments): LPAREN
	// follow(arguments): SEMICOLON
    
	DEBUG_PRINT(("Parsing arguments\n"));
    
	TOKEN_TYPE expected[] = {LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {EOF_TYPE, SEMICOLON_TYPE};
    
	switch(tok.type) {
        case LPAREN_TYPE:
		{
            // Grammar production: LPAREN parameter_list RPAREN
            if (!match(LPAREN_TYPE))
                goto synch;
            type parameter_list_type = parse_parameter_list(procedure_node);
            if (!match(RPAREN_TYPE))
                goto synch;
            if (parameter_list_type == ERROR_STAR || parameter_list_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "arguments", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_parameter_list(stack_node *procedure_node) {
	// first(parameter_list): ID
	// follow(parameter_list): RPAREN
    
	DEBUG_PRINT(("Parsing parameter_list\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
		{
            // Grammar production: identifier_list COLON type parameter_list_2
            type identifier_list_type = parse_identifier_list(procedure_node);
            // parse_identifier_list() should have set some parameters
            assert(procedure_node->parameters);
            if (!match(COLON_TYPE))
                goto synch;
            // Ignore returned size, not needed for parameters
            int size = 0;
            type type_type = parse_type(&size);
            type param_type = type_type;
            if (param_type == ERROR_STAR) {
                param_type = ERROR;
            }
            stack_node *last_param = set_parameter_types(procedure_node->parameters, param_type);
            type parameter_list_2_type = parse_parameter_list_2(last_param);
            if (identifier_list_type == ERROR_STAR || identifier_list_type == ERROR || type_type == ERROR_STAR || type_type == ERROR || parameter_list_2_type == ERROR_STAR || parameter_list_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "parameter_list", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_parameter_list_2(stack_node *prev_param) {
	// first(parameter_list_2): EPSILON, SEMICOLON
	// follow(parameter_list_2): RPAREN
    
	DEBUG_PRINT(("Parsing parameter_list_2\n"));
    
	TOKEN_TYPE expected[] = {RPAREN_TYPE, SEMICOLON_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return NONE;
        case SEMICOLON_TYPE:
		{
            // Grammar production: SEMICOLON identifier_list COLON type parameter_list_2
            if (!match(SEMICOLON_TYPE))
                goto synch;
            type identifier_list_type = parse_identifier_list(prev_param);
            // parse_identifier_list() should have set some parameters
            assert(prev_param->link);
            if (!match(COLON_TYPE))
                goto synch;
            // Ignore returned size, not needed for parameters
            int size;
            type type_type = parse_type(&size);
            type param_type = type_type;
            if (param_type == ERROR_STAR) {
                param_type = ERROR;
            }
            stack_node *last_param = set_parameter_types(prev_param->link, param_type);
            type parameter_list_2_type = parse_parameter_list_2(last_param);
            if (identifier_list_type == ERROR_STAR || identifier_list_type == ERROR || type_type == ERROR_STAR || type_type == ERROR || parameter_list_2_type == ERROR_STAR || parameter_list_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "parameter_list_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_compound_statement() {
	// first(compound_statement): BEGIN
	// follow(compound_statement): END, PERIOD, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing compound_statement\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE};
	TOKEN_TYPE synch_set[] = {ELSE_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, PERIOD_TYPE};
    
	switch(tok.type) {
        case BEGIN_TYPE:
		{
            // Grammar production: BEGIN compound_statement_2
            if (!match(BEGIN_TYPE))
                goto synch;
            type compound_statement_2_type = parse_compound_statement_2();
            if (compound_statement_2_type == ERROR_STAR || compound_statement_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "compound_statement", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_compound_statement_2() {
	// first(compound_statement_2): BEGIN, END, CALL, IF, WHILE, ID
	// follow(compound_statement_2): END, PERIOD, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing compound_statement_2\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, END_TYPE, WHILE_TYPE, CALL_TYPE, ID_TYPE, IF_TYPE};
	TOKEN_TYPE synch_set[] = {ELSE_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, PERIOD_TYPE};
    
	switch(tok.type) {
        case END_TYPE:
		{
            // Grammar production: END
            if (!match(END_TYPE))
                goto synch;
            return NONE;
		}
        case BEGIN_TYPE:
        case WHILE_TYPE:
        case CALL_TYPE:
        case ID_TYPE:
        case IF_TYPE:
		{
            // Grammar production: optional_statements END
            type optional_statements_type = parse_optional_statements();
            if (!match(END_TYPE))
                goto synch;
            if (optional_statements_type == ERROR_STAR || optional_statements_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "compound_statement_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_optional_statements() {
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
		{
            // Grammar production: statement_list
            type statement_list_type = parse_statement_list();
            if (statement_list_type == ERROR_STAR || statement_list_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "optional_statements", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_statement_list() {
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
		{
            // Grammar production: statement statement_list_2
            type statement_type = parse_statement();
            type statement_list_2_type = parse_statement_list_2();
            if (statement_type == ERROR_STAR || statement_type == ERROR || statement_list_2_type == ERROR_STAR || statement_list_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "statement_list", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_statement_list_2() {
	// first(statement_list_2): EPSILON, SEMICOLON
	// follow(statement_list_2): END
    
	DEBUG_PRINT(("Parsing statement_list_2\n"));
    
	TOKEN_TYPE expected[] = {END_TYPE, SEMICOLON_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case END_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            return NONE;
        case SEMICOLON_TYPE:
		{
            // Grammar production: SEMICOLON statement statement_list_2
            if (!match(SEMICOLON_TYPE))
                goto synch;
            type statement_type = parse_statement();
            type statement_list_2_type = parse_statement_list_2();
            if (statement_type == ERROR_STAR || statement_type == ERROR || statement_list_2_type == ERROR_STAR || statement_list_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "statement_list_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_statement() {
	// first(statement): WHILE, CALL, IF, BEGIN, ID
	// follow(statement): END, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing statement\n"));
    
	TOKEN_TYPE expected[] = {BEGIN_TYPE, CALL_TYPE, ID_TYPE, WHILE_TYPE, IF_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case IF_TYPE:
		{
            // Grammar production: IF expression THEN statement statement_2
            if (!match(IF_TYPE))
                goto synch;
            type expression_type = parse_expression();
            if (expression_type != ERROR_STAR && expression_type != ERROR && expression_type != BOOLEAN) {
                write_listing_symerr("If statement condition expression is not boolean", NULL);
            }
            if (!match(THEN_TYPE))
                goto synch;
            type statement_type = parse_statement();
            type statement_2_type = parse_statement_2();
            if (expression_type == ERROR_STAR || expression_type == ERROR || statement_type == ERROR_STAR || statement_type == ERROR || statement_2_type == ERROR_STAR || statement_2_type == ERROR)
                return ERROR;
            if (expression_type != BOOLEAN)
                return ERROR_STAR;
            return NONE;
		}
        case WHILE_TYPE:
		{
            // Grammar production: WHILE expression DO statement
            if (!match(WHILE_TYPE))
                goto synch;
            type expression_type = parse_expression();
            if (expression_type != ERROR_STAR && expression_type != ERROR && expression_type != BOOLEAN) {
                write_listing_symerr("While statement condition expression is not boolean", NULL);
            }
            if (!match(DO_TYPE))
                goto synch;
            type statement_type = parse_statement();
            if (expression_type == ERROR_STAR || expression_type == ERROR || statement_type == ERROR_STAR || statement_type == ERROR)
                return ERROR;
            if (expression_type != BOOLEAN)
                return ERROR_STAR;
            return NONE;
		}
        case BEGIN_TYPE:
		{
            // Grammar production: compound_statement
            type compound_statement_type = parse_compound_statement();
            if (compound_statement_type == ERROR_STAR || compound_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        case CALL_TYPE:
		{
            // Grammar production: procedure_statement
            type procedure_statement_type = parse_procedure_statement();
            if (procedure_statement_type == ERROR_STAR || procedure_statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        case ID_TYPE:
		{
            // Grammar production: variable ASSIGNOP expression
            type variable_type = parse_variable();
            if (!match(ASSIGNOP_TYPE))
                goto synch;
            type expression_type = parse_expression();
            if (variable_type == ERROR_STAR || variable_type == ERROR || expression_type == ERROR_STAR || expression_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "statement", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_statement_2() {
	// first(statement_2): EPSILON, ELSE
	// follow(statement_2): END, SEMICOLON, ELSE
	// MULTIPLE PRODUCTIONS FOR SYMBOL: ELSE
    
	DEBUG_PRINT(("Parsing statement_2\n"));
    
	TOKEN_TYPE expected[] = {END_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case ELSE_TYPE:
		{
            // Grammar production: ELSE statement
            if (!match(ELSE_TYPE))
                goto synch;
            type statement_type = parse_statement();
            if (statement_type == ERROR_STAR || statement_type == ERROR)
                return ERROR;
            return NONE;
		}
        case END_TYPE:
        case SEMICOLON_TYPE:
        // case ELSE_TYPE: -- resolve dangling else
            // Grammar production: EPSILON
            // Epslion production
            return NONE;
        default:
            write_listing_synerr(lineno, tok, "statement_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_variable() {
	// first(variable): ID
	// follow(variable): ASSIGNOP
    
	DEBUG_PRINT(("Parsing variable\n"));
    
	TOKEN_TYPE expected[] = {ID_TYPE};
	TOKEN_TYPE synch_set[] = {ASSIGNOP_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ID_TYPE:
		{
            // Grammar production: ID variable_2
            token var_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            stack_node *var_node = check_id(var_tok.lexeme, false);
            if (!var_node) {
                write_listing_symerr("Use of undeclared identifier \"%s\"", var_tok.lexeme);
                // Ignore return type of parse_variable_2(), error regardless
                parse_variable_2(NULL);
                return ERROR_STAR;
            }
            if (var_node->id_type == PROCEDURE) {
                write_listing_symerr("Procedure \"%s\" cannot be used for assignment", var_tok.lexeme);
                // Ignore return type of parse_variable_2(), error regardless
                parse_variable_2(var_node);
                return ERROR_STAR;
            }
            type variable_2_type = parse_variable_2(var_node);
            if (variable_2_type == ERROR_STAR || variable_2_type == ERROR)
                return ERROR;
            return variable_2_type;
		}
        default:
            write_listing_synerr(lineno, tok, "variable", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_variable_2(stack_node *var_node) {
	// first(variable_2): LBRACKET, EPSILON
	// follow(variable_2): ASSIGNOP
    
	DEBUG_PRINT(("Parsing variable_2\n"));
    
	TOKEN_TYPE expected[] = {LBRACKET_TYPE, ASSIGNOP_TYPE};
	TOKEN_TYPE synch_set[] = {ASSIGNOP_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case ASSIGNOP_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            if (!var_node) {
                return ERROR;
            }
            return var_node->id_type;
        case LBRACKET_TYPE:
		{
            bool error = false;
            if (var_node && !(var_node->id_type == ARRAY_INTEGER || var_node->id_type == ARRAY_REAL)) {
                write_listing_symerr("Identifier \"%s\" is not an array type", var_node->id);
                error = true;
            }
            // Grammar production: LBRACKET expression RBRACKET
            if (!match(LBRACKET_TYPE))
                goto synch;
            type expression_type = parse_expression();
            if (expression_type != INTEGER) {
                write_listing_symerr("Array subscript must be an integer expression", NULL);
                error = true;
            }
            if (!match(RBRACKET_TYPE))
                goto synch;
            if (error)
                return ERROR_STAR;
            if (!var_node || expression_type == ERROR_STAR || expression_type == ERROR)
                return ERROR;
            if (var_node->id_type == ARRAY_INTEGER) {
                return INTEGER;
            } else if (var_node->id_type == ARRAY_REAL) {
                return REAL;
            }
            // Type of var_node is PROCEDURE, could be ERROR instead
            return ERROR_STAR;
		}
        default:
            write_listing_synerr(lineno, tok, "variable_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_procedure_statement() {
	// first(procedure_statement): CALL
	// follow(procedure_statement): END, SEMICOLON, ELSE
    
	DEBUG_PRINT(("Parsing procedure_statement\n"));
    
	TOKEN_TYPE expected[] = {CALL_TYPE};
	TOKEN_TYPE synch_set[] = {END_TYPE, EOF_TYPE, SEMICOLON_TYPE, ELSE_TYPE};
    
	switch(tok.type) {
        case CALL_TYPE:
		{
            // Grammar production: CALL ID procedure_statement_2
            if (!match(CALL_TYPE))
                goto synch;
            token id_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            stack_node *procedure_node = check_id(id_tok.lexeme, false);
            if (!procedure_node) {
                write_listing_symerr("Procedure \"%s\" does not exist", id_tok.lexeme);
                // Ignore return type of parse_procedure_statement_2(), error regardless
                parse_procedure_statement_2(NULL);
                return ERROR_STAR;
            }
            if (procedure_node->id_type != PROCEDURE) {
                write_listing_symerr("Identifier \"%s\" is not a procedure", id_tok.lexeme);
                // Ignore return type of parse_procedure_statement_2(), error regardless
                parse_procedure_statement_2(NULL);
                return ERROR_STAR;
            }
            type procedure_statement_2_type = parse_procedure_statement_2(procedure_node);
            if (procedure_statement_2_type == ERROR_STAR || procedure_statement_2_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "procedure_statement", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_procedure_statement_2(stack_node *procedure_node) {
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
            if (procedure_node && procedure_node->parameters != NULL) {
                write_listing_symerr("Procedure \"%s\" called without required parameters", procedure_node->id);
                return ERROR_STAR;
            }
            return NONE;
        case LPAREN_TYPE:
		{
            // Grammar production: LPAREN expression_list RPAREN
            if (!match(LPAREN_TYPE))
                goto synch;
            type expression_list_type = parse_expression_list(procedure_node);
            if (!match(RPAREN_TYPE))
                goto synch;
            if (expression_list_type == ERROR_STAR || expression_list_type == ERROR)
                return ERROR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "procedure_statement_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_expression_list(stack_node *procedure_node) {
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
		{
            // Grammar production: expression expression_list_2
            bool error = false;
            stack_node *parameter_node = NULL;
            stack_node *next_parameter_node = NULL;
            if (procedure_node) {
                // Allow NULL procedure_node if procedure does not exist
                parameter_node = procedure_node->parameters;

                if (!parameter_node) {
                    write_listing_symerr("Procedure \"%s\" called with parameters, but defined with none", procedure_node->id);
                    error = true;
                } else {
                    next_parameter_node = parameter_node->link;
                }
            }
            type expression_type = parse_expression();
            if (procedure_node && parameter_node && expression_type != ERROR_STAR && expression_type != ERROR && expression_type != parameter_node->id_type) {
                // TODO: include types in error message
                write_listing_symerr("Procedure \"%s\" called with parameter of incorrect type", procedure_node->id);
                error = true;
            }
            type expression_list_2_type = parse_expression_list_2(procedure_node, next_parameter_node);
            if (expression_type == ERROR_STAR || expression_type == ERROR || expression_list_2_type == ERROR_STAR || expression_list_2_type == ERROR)
                return ERROR;
            if (error)
                return ERROR_STAR;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "expression_list", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_expression_list_2(stack_node *procedure_node, stack_node *parameter_node) {
	// first(expression_list_2): EPSILON, COMMA
	// follow(expression_list_2): RPAREN
    
	DEBUG_PRINT(("Parsing expression_list_2\n"));
    
	TOKEN_TYPE expected[] = {RPAREN_TYPE, COMMA_TYPE};
	TOKEN_TYPE synch_set[] = {RPAREN_TYPE, EOF_TYPE};
    
	switch(tok.type) {
        case COMMA_TYPE:
		{
            // Grammar production: COMMA expression expression_list_2
            bool error = false;
            stack_node *next_parameter_node = NULL;
            if (procedure_node) {
                if (!parameter_node) {
                    write_listing_symerr("Procedure \"%s\" called with too many parameters", procedure_node->id);
                    error = true;
                } else {
                    next_parameter_node = parameter_node->link;
                }
            }
            if (!match(COMMA_TYPE))
                goto synch;
            type expression_type = parse_expression();
            if (procedure_node && parameter_node && expression_type != ERROR_STAR && expression_type != ERROR && expression_type != parameter_node->id_type) {
                // TODO: include types in error message
                write_listing_symerr("Procedure \"%s\" called with parameter of incorrect type", procedure_node->id);
                error = true;
            }
            type expression_list_2_type = parse_expression_list_2(procedure_node, next_parameter_node);
            if (expression_type == ERROR_STAR || expression_type == ERROR || expression_list_2_type == ERROR_STAR || expression_list_2_type == ERROR)
                return ERROR;
            if (error)
                return ERROR_STAR;
            return NONE;
		}
        case RPAREN_TYPE:
            // Grammar production: EPSILON
            // Epslion production
            if (parameter_node) {
                write_listing_symerr("Procedure \"%s\" called with too few parameters", procedure_node->id);
                return ERROR_STAR;
            }
            return NONE;
        default:
            write_listing_synerr(lineno, tok, "expression_list_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_expression() {
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
		{
            // Grammar production: simple_expression expression_2
            type simple_expression_type = parse_simple_expression();
            type expression_2_type = parse_expression_2(simple_expression_type);
            if (simple_expression_type == ERROR_STAR || simple_expression_type == ERROR || expression_2_type == ERROR_STAR || expression_2_type == ERROR)
                return ERROR;
            return expression_2_type;
		}
        default:
            write_listing_synerr(lineno, tok, "expression", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_expression_2(type prev_simple_expression_type) {
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
            if (prev_simple_expression_type == ERROR_STAR || prev_simple_expression_type == ERROR)
                return ERROR;
            return prev_simple_expression_type;
        case RELOP_TYPE:
		{
            // Grammar production: RELOP simple_expression
            if (!match(RELOP_TYPE))
                goto synch;
            type simple_expression_type = parse_simple_expression();
            if (prev_simple_expression_type == ERROR_STAR || prev_simple_expression_type == ERROR || simple_expression_type == ERROR_STAR || simple_expression_type == ERROR)
                return ERROR;
            // TODO: could allow boolean == boolean or boolean != boolean
            if ((prev_simple_expression_type != INTEGER && prev_simple_expression_type != REAL) || (simple_expression_type != INTEGER && simple_expression_type != REAL)) {
                write_listing_symerr("Relop operands must be numeric", NULL);
                return ERROR_STAR;
            }
            return BOOLEAN;
		}
        default:
            write_listing_synerr(lineno, tok, "expression_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_simple_expression() {
	// first(simple_expression): NUM, PLUS, LPAREN, NOT, ID, MINUS
	// follow(simple_expression): DO, END, RELOP, SEMICOLON, THEN, ELSE, COMMA, RPAREN, RBRACKET
    
	DEBUG_PRINT(("Parsing simple_expression\n"));
    
	TOKEN_TYPE expected[] = {MINUS_TYPE, NUM_TYPE, PLUS_TYPE, LPAREN_TYPE, NOT_TYPE, ID_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE, RELOP_TYPE};
    
	switch(tok.type) {
        case MINUS_TYPE:
        case PLUS_TYPE:
		{
            // Grammar production: sign term simple_expression_2
            type sign_type = parse_sign();
            type term_type = parse_term();
            if (term_type != ERROR_STAR && term_type != ERROR && term_type != INTEGER && term_type != REAL) {
                write_listing_symerr("Sign operand must be numeric", NULL);
                parse_simple_expression_2(ERROR);
                return ERROR_STAR;
            }
            type simple_expression_2_type = parse_simple_expression_2(term_type);
            if (sign_type == ERROR_STAR || sign_type == ERROR || term_type == ERROR_STAR || term_type == ERROR || simple_expression_2_type == ERROR_STAR || simple_expression_2_type == ERROR)
                return ERROR;
            return simple_expression_2_type;
		}
        case NUM_TYPE:
        case LPAREN_TYPE:
        case NOT_TYPE:
        case ID_TYPE:
		{
            // Grammar production: term simple_expression_2
            type term_type = parse_term();
            type simple_expression_2_type = parse_simple_expression_2(term_type);
            if (term_type == ERROR_STAR || term_type == ERROR || simple_expression_2_type == ERROR_STAR || simple_expression_2_type == ERROR)
                return ERROR;
            return simple_expression_2_type;
		}
        default:
            write_listing_synerr(lineno, tok, "simple_expression", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_simple_expression_2(type prev_term_type) {
	// first(simple_expression_2): ADDOP, PLUS, MINUS, EPSILON
	// follow(simple_expression_2): DO, END, RELOP, SEMICOLON, THEN, ELSE, COMMA, RPAREN, RBRACKET
    
	DEBUG_PRINT(("Parsing simple_expression_2\n"));
    
	TOKEN_TYPE expected[] = {ADDOP_TYPE, END_TYPE, RELOP_TYPE, SEMICOLON_TYPE, DO_TYPE, THEN_TYPE, ELSE_TYPE, PLUS_TYPE, COMMA_TYPE, RBRACKET_TYPE, MINUS_TYPE, RPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, COMMA_TYPE, RPAREN_TYPE, RBRACKET_TYPE, RELOP_TYPE};
    
	switch(tok.type) {
        case ADDOP_TYPE:
		{
            // Grammar production: ADDOP term simple_expression_2
            token addop_tok = tok;
            if (!match(ADDOP_TYPE))
                goto synch;
            type term_type = parse_term();
            type simple_expression_2_type;
            if (addop_tok.attr.addop == OR_ADDOP) {
                if (!(term_type == ERROR_STAR || term_type == ERROR || prev_term_type == ERROR_STAR || prev_term_type == ERROR) && (prev_term_type != BOOLEAN || term_type != BOOLEAN)) {
                    write_listing_symerr("Addop OR must be applied to boolean operands", NULL);
                    parse_simple_expression_2(ERROR);
                    return ERROR_STAR;
                }
                simple_expression_2_type = parse_simple_expression_2(BOOLEAN);
            } else {
                if (!(term_type == ERROR_STAR || term_type == ERROR || prev_term_type == ERROR_STAR || prev_term_type == ERROR) && !(term_type == INTEGER || term_type == REAL) && !(prev_term_type == INTEGER || prev_term_type == REAL)) {
                    write_listing_symerr("Addop \"%s\" must be applied to numeric operands", addop_tok.lexeme);
                    parse_simple_expression_2(ERROR);
                    return ERROR_STAR;
                }
                if (prev_term_type == INTEGER && term_type == INTEGER) {
                    simple_expression_2_type = parse_simple_expression_2(INTEGER);
                } else {
                    simple_expression_2_type = parse_simple_expression_2(REAL);
                }
            }
            if (prev_term_type == ERROR_STAR || prev_term_type == ERROR || term_type == ERROR_STAR || term_type == ERROR || simple_expression_2_type == ERROR_STAR || simple_expression_2_type == ERROR)
                return ERROR;
            return simple_expression_2_type;
		}
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
            if (prev_term_type == ERROR_STAR || prev_term_type == ERROR)
                return ERROR;
            return prev_term_type;
        case MINUS_TYPE:
		{
            // Grammar production: MINUS term simple_expression_2
            if (!match(MINUS_TYPE))
                goto synch;
            type term_type = parse_term();
            if (!(term_type == ERROR_STAR || term_type == ERROR || prev_term_type == ERROR_STAR || prev_term_type == ERROR) && !(term_type == INTEGER || term_type == REAL) && !(prev_term_type == INTEGER || prev_term_type == REAL)) {
                write_listing_symerr("Addop \"-\" must be applied to numeric operands", NULL);
                parse_simple_expression_2(ERROR);
                return ERROR_STAR;
            }
            type simple_expression_2_type;
            if (prev_term_type == INTEGER && term_type == INTEGER) {
                simple_expression_2_type = parse_simple_expression_2(INTEGER);
            } else {
                simple_expression_2_type = parse_simple_expression_2(REAL);
            }
            if (prev_term_type == ERROR_STAR || prev_term_type == ERROR || term_type == ERROR_STAR || term_type == ERROR || simple_expression_2_type == ERROR_STAR || simple_expression_2_type == ERROR)
                return ERROR;
            return simple_expression_2_type;
		}
        case PLUS_TYPE: 
		{
            // Grammar production: PLUS term simple_expression_2
            if (!match(PLUS_TYPE))
                goto synch;
            type term_type = parse_term();
            if (!(term_type == ERROR_STAR || term_type == ERROR || prev_term_type == ERROR_STAR || prev_term_type == ERROR) && !(term_type == INTEGER || term_type == REAL) && !(prev_term_type == INTEGER || prev_term_type == REAL)) {
                write_listing_symerr("Addop \"+\" must be applied to numeric operands", NULL);
                parse_simple_expression_2(ERROR);
                return ERROR_STAR;
            }
            type simple_expression_2_type;
            if (prev_term_type == INTEGER && term_type == INTEGER) {
                simple_expression_2_type = parse_simple_expression_2(INTEGER);
            } else {
                simple_expression_2_type = parse_simple_expression_2(REAL);
            }
            if (prev_term_type == ERROR_STAR || prev_term_type == ERROR || term_type == ERROR_STAR || term_type == ERROR || simple_expression_2_type == ERROR_STAR || simple_expression_2_type == ERROR)
                return ERROR;
            return simple_expression_2_type;
		}
        default:
            write_listing_synerr(lineno, tok, "simple_expression_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_term() {
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
		{
            // Grammar production: factor term_2
            type factor_type = parse_factor();
            type term_2_type = parse_term_2(factor_type);
            if (factor_type == ERROR_STAR || factor_type == ERROR || term_2_type == ERROR_STAR || term_2_type == ERROR)
                return ERROR;
            return term_2_type;
		}
        default:
            write_listing_synerr(lineno, tok, "term", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_term_2(type prev_factor_type) {
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
            if (prev_factor_type == ERROR_STAR)
                return ERROR;
            return prev_factor_type;
        case MULOP_TYPE: 
		{
            // Grammar production: MULOP factor term_2
            token mulop_tok = tok;
            if (!match(MULOP_TYPE))
                goto synch;
            type factor_type = parse_factor();
            type term_2_type;
            if (mulop_tok.attr.mulop == AND_MULOP) {
                if (!(prev_factor_type == ERROR_STAR || prev_factor_type == ERROR || factor_type == ERROR_STAR || factor_type == ERROR) && prev_factor_type != BOOLEAN && factor_type != BOOLEAN) {
                    write_listing_symerr("Mulop AND must be applied to boolean operands", NULL);
                    parse_term_2(ERROR);
                    return ERROR_STAR;
                }
                term_2_type = parse_term_2(BOOLEAN);
            } else {
                if (!(prev_factor_type == ERROR_STAR || prev_factor_type == ERROR || factor_type == ERROR_STAR || factor_type == ERROR) && !(prev_factor_type == INTEGER || prev_factor_type == REAL) && !(factor_type == INTEGER || factor_type == REAL)) {
                    write_listing_symerr("Mulop \"%s\" must be applied to numeric operands", mulop_tok.lexeme);
                    parse_term_2(ERROR);
                    return ERROR_STAR;
                }
                if (prev_factor_type == INTEGER && factor_type == INTEGER) {
                    term_2_type = parse_term_2(INTEGER);
                } else {
                    term_2_type = parse_term_2(REAL);
                }
            }
            if (prev_factor_type == ERROR_STAR || prev_factor_type == ERROR || factor_type == ERROR_STAR || factor_type == ERROR || term_2_type == ERROR_STAR || term_2_type == ERROR)
                return ERROR;
            return term_2_type;
		}
        default:
            write_listing_synerr(lineno, tok, "term_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_factor() {
	// first(factor): NOT, NUM, ID, LPAREN
	// follow(factor): DO, END, RELOP, THEN, ELSE, MULOP, MINUS, ADDOP, RPAREN, SEMICOLON, PLUS, COMMA, RBRACKET
    
	DEBUG_PRINT(("Parsing factor\n"));
    
	TOKEN_TYPE expected[] = {NOT_TYPE, NUM_TYPE, ID_TYPE, LPAREN_TYPE};
	TOKEN_TYPE synch_set[] = {DO_TYPE, END_TYPE, EOF_TYPE, SEMICOLON_TYPE, THEN_TYPE, ELSE_TYPE, ADDOP_TYPE, PLUS_TYPE, MULOP_TYPE, RPAREN_TYPE, RBRACKET_TYPE, COMMA_TYPE, MINUS_TYPE, RELOP_TYPE};
    
	switch(tok.type) {
        case ID_TYPE: 
		{
            // Grammar production: ID factor_2
            token id_tok = tok;
            if (!match(ID_TYPE))
                goto synch;
            stack_node *id_node = check_id(id_tok.lexeme, false);
            if (!id_node) {
                write_listing_symerr("Use of undeclared identifier \"%s\"", id_tok.lexeme);
                parse_factor_2(NULL);
                return ERROR_STAR;
            }
            type factor_2_type = parse_factor_2(id_node);
            if (factor_2_type == ERROR_STAR || factor_2_type == ERROR)
                return ERROR;
            return factor_2_type;
		}
        case LPAREN_TYPE: 
		{
            // Grammar production: LPAREN expression RPAREN
            if (!match(LPAREN_TYPE))
                goto synch;
            type expression_type = parse_expression();
            if (!match(RPAREN_TYPE))
                goto synch;
            if (expression_type == ERROR_STAR || expression_type == ERROR)
                return ERROR;
            return expression_type;
		}
        case NOT_TYPE: 
		{
            // Grammar production: NOT factor
            if (!match(NOT_TYPE))
                goto synch;
            type factor_type = parse_factor();
            if (!(factor_type == ERROR_STAR || factor_type == ERROR) && factor_type != BOOLEAN) {
                write_listing_symerr("NOT must be applied to boolean operand", NULL);
                return ERROR_STAR;
            }
            if (factor_type == ERROR_STAR || factor_type == ERROR)
                return ERROR;
            return BOOLEAN;
		}
        case NUM_TYPE: 
		{
            // Grammar production: NUM
            token num_tok = tok;
            if (!match(NUM_TYPE))
                goto synch;
            if (num_tok.attr.num == INTEGER_NUM) {
                return INTEGER;
            }
            return REAL;
		}
        default:
            write_listing_synerr(lineno, tok, "factor", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_factor_2(stack_node *id_node) {
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
            if (!id_node) {
                return ERROR;
            }
            // TODO: should this be checked? Need to allow passing arrays to functions though
//            else if (!(id_node->id_type == INTEGER || id_node->id_type == REAL)) {
//                write_listing_symerr("The type of the identifier \"%s\" cannot be used as a factor", id_node->id);
//                return ERROR_STAR;
//            }
            return id_node->id_type;
        case LBRACKET_TYPE: 
		{
            bool error = false;
            // Grammar production: LBRACKET expression RBRACKET
            if (!match(LBRACKET_TYPE))
                goto synch;
            if (id_node && !(id_node->id_type == ARRAY_INTEGER || id_node->id_type == ARRAY_REAL)) {
                write_listing_symerr("Identifier \"%s\" is not an array", id_node->id);
            }
            type expression_type = parse_expression();
            if (expression_type != INTEGER) {
                write_listing_symerr("Array subscript must be an integer expression", NULL);
                error = true;
            }
            if (!match(RBRACKET_TYPE))
                goto synch;
            if (!id_node || expression_type == ERROR_STAR || expression_type == ERROR)
                return ERROR;
            if (error || !(id_node->id_type == ARRAY_INTEGER || id_node->id_type == ARRAY_REAL)) {
                return ERROR_STAR;
            }
            if (id_node->id_type == ARRAY_INTEGER) {
                return INTEGER;
            }
            return REAL;
		}
        default:
            write_listing_synerr(lineno, tok, "factor_2", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

type parse_sign() {
	// first(sign): PLUS, MINUS
	// follow(sign): NOT, NUM, ID, LPAREN
    
	DEBUG_PRINT(("Parsing sign\n"));
    
	TOKEN_TYPE expected[] = {PLUS_TYPE, MINUS_TYPE};
	TOKEN_TYPE synch_set[] = {NOT_TYPE, NUM_TYPE, EOF_TYPE, LPAREN_TYPE, ID_TYPE};
    
	switch(tok.type) {
        case MINUS_TYPE: 
		{
            // Grammar production: MINUS
            if (!match(MINUS_TYPE))
                goto synch;
            return NONE;
		}
        case PLUS_TYPE: 
		{
            // Grammar production: PLUS
            if (!match(PLUS_TYPE))
                goto synch;
            return NONE;
		}
        default:
            write_listing_synerr(lineno, tok, "sign", expected, sizeof(expected)/sizeof(expected[0]));
            break;
	}
    
synch:
	synch(synch_set, sizeof(synch_set)/sizeof(synch_set[0]));
	return ERROR_STAR;
}

