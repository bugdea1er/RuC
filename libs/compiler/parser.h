/*
 *	Copyright 2021 Andrey Terekhov, Ilya Andreev
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 */

#pragma once

#include <stdbool.h>
#include "errors.h"
#include "lexer.h"
#include "stack.h"
#include "syntax.h"
#include "tree.h"
#include "operations.h"
#include "uniio.h"
#include "workspace.h"


#ifdef __cplusplus
extern "C" {
#endif

/**	The kind of block to parse */
typedef enum BLOCK
{
	REGBLOCK,
	THREAD,
	FUNCBODY,
	FORBLOCK,
} block_t;


typedef struct location
{
	size_t begin;
	size_t end;
} location_t;

/** Parser structure */
typedef struct parser
{
	syntax *sx;							/**< Syntax structure */
	lexer *lxr;							/**< Lexer structure */

	vector labels;						/**< Labels table */

	node nd;							/**< Node for expression subtree */

	token_t token;						/**< Current token */
	size_t function_mode;				/**< Mode of current parsed function */
	size_t array_dimensions;			/**< Array dimensions counter */

	int func_def;						/**< @c 0 for function without arguments,
											@c 1 for function definition,
											@c 2 for function declaration,
											@c 3 for others */

	int flag_strings_only;				/**< @c 0 for non-string initialization,
											@c 1 for string initialization,
											@c 2 for parsing before initialization */

	int flag_array_in_struct;			/**< Set, if parsed struct declaration has an array */
	int flag_empty_bounds;				/**< Set, if array declaration has empty bounds */

	bool is_in_switch;					/**< Set, if parser is in switch body */
	bool is_in_loop;					/**< Set, if parser is in loop body */

	bool was_return;					/**< Set, if was return in parsed function */
	bool was_type_def;					/**< Set, if was type definition */
	bool was_error;						/**< Set, if was error */
} parser;

/** Expression structure */
typedef struct expression
{
	bool is_valid;			/**< Set if is valid */
	location_t location;	/**< Source location */
	node nd;				/**< Node in AST */
} expression;


/**
 *	Parse source code to generate syntax structure
 *
 *	@param	ws			Compiler workspace
 *	@param	io			Universal io structure
 *	@param	sx			Syntax structure
 *
 *	@return	@c 0 on success, @c 1 on failure
 */
int parse(const workspace *const ws, universal_io *const io, syntax *const sx);


/**
 *	Emit a syntax error from parser
 *
 *	@param	prs			Parser structure
 *	@param	num			Error code
 */
void parser_error(parser *const prs, error_t num, ...);

/**
 *	Emit a semantics error from parser
 *
 *	@param	prs			Parser structure
 *	@param	loc			Error location
 *	@param	num			Error code
 */
void semantics_error(parser *const prs, const location_t loc, error_t num, ...);


/**
 *	Consume the current 'peek token' and lex the next one
 *
 *	@param	prs			Parser structure
 */
location_t token_consume(parser *const prs);

/**
 *	Try to consume the current 'peek token' and lex the next one
 *
 *	@param	prs			Parser structure
 *	@param	expected	Expected token to consume
 *
 *	@return	@c 1 on consuming 'peek token', @c 0 on otherwise
 */
int token_try_consume(parser *const prs, const token_t expected);

/**
 *	Try to consume the current 'peek token' and lex the next one
 *	If 'peek token' is expected, parser will consume it, otherwise an error will be emitted
 *
 *	@param	prs			Parser structure
 *	@param	expected	Expected token to consume
 *	@param	err			Error to emit
 */
void token_expect_and_consume(parser *const prs, const token_t expected, const error_t err);

/**
 *	Read tokens until one of the specified tokens
 *
 *	@param	prs			Parser structure
 *	@param	tokens		Set of specified tokens
 */
void token_skip_until(parser *const prs, const uint8_t tokens);


/**
 *	Parse assignment expression [C99 6.5.16]
 *
 *	assignment-expression:
 *		conditional-expression
 *		unary-expression assignment-operator assignment-expression
 *
 *	assignment-operator: one of
 *		'=' '*=' '/=' '%=' '+=' '-=' '<<=' '>>=' '&=' 'ˆ=' '|='
 *
 *	@param	prs			Parser
 *
 *	@return	Assignment expression
 */
expression parse_assignment_expression(parser *const prs);

/**
 *	Parse expression [C99 6.5.17]
 *
 *	expression:
 *		assignment-expression
 *		expression ',' assignment-expression
 *
 *	@param	prs			Parser
 *
 *	@return Expression
 */
expression parse_expression(parser *const prs);

/**
 *	Parse constant expression [C99 6.6]
 *
 *	constant-expression:
 *		conditional-expression
 *
 *	@param	prs			Parser
 *
 *	@return	Constant expression
 */
expression parse_constant_expression(parser *const prs);


/**
 *	Parse a declaration [C99 6.7]
 *	@note Parses a full declaration, which consists of declaration-specifiers,
 *	some number of declarators, and a semicolon
 *
 *	@param	prs			Parser structure
 *	@param	parent		Parent node in AST
 */
void parse_declaration_inner(parser *const prs, node *const parent);

/**
 *	Parse a top level declaration [C99 6.7]
 *	@note Parses a full declaration, which consists either of declaration-specifiers,
 *	some number of declarators, and a semicolon, or function definition
 *
 *	@param	prs			Parser structure
 *	@param	root		Root node in AST
 */
void parse_declaration_external(parser *const prs, node *const root);

/**
 *	Parse initializer [C99 6.7.8]
 *
 *	initializer:
 *		assignment-expression
 *		'{' initializer-list '}'
 *
 *	@param	prs			Parser structure
 *	@param	parent		Parent node in AST
 *	@param	type		Type of variable in declaration
 */
void parse_initializer(parser *const prs, node *const parent, const item_t type);


/**
 *	Parse statement [C99 6.8]
 *
 *	statement:
 *		labeled-statement
 *		compound-statement
 *		expression-statement
 *		selection-statement
 *		iteration-statement
 *		jump-statement
 *
 *	@param	prs			Parser structure
 *	@param	parent		Parent node in AST
 */
void parse_statement(parser *const prs, node *const parent);

/**
 *	Parse '{}' block [C99 6.8.2]
 *
 *	compound-statement:
 *  	{ block-item-list[opt] }
 *
 *	block-item-list:
 *		block-item
 *		block-item-list block-item
 *
 *	block-item:
 *		declaration
 *		statement
 *
 *	@param	prs			Parser structure
 *	@param	parent		Parent node in AST
 */
void parse_statement_compound(parser *const prs, node *const parent, const block_t type);


/**
 *	Add new item to identifiers table
 *
 *	@param	prs			Parser structure
 *	@param	repr		New identifier index in representations table
 *	@param	type		@c -1 for function as parameter,
 *						@c  0 for variable,
 *						@c  1 for label,
 *						@c  funcnum for function,
 *						@c  >= @c 100 for type specifier
 *	@param	mode		New identifier mode
 *
 *	@return	Index of the last item in identifiers table
 */
size_t to_identab(parser *const prs, const size_t repr, const item_t type, const item_t mode);

#ifdef __cplusplus
} /* extern "C" */
#endif