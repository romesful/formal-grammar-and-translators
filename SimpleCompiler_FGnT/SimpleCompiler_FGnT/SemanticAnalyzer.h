#pragma once
#include "ErrorHandler.h"
#include "Token.h"
#include "Type.h"
#include <vector>
#include <map>
#include <iostream>
#include <string>

using namespace std;

class SemanticAnalyzer
{
public:
	SemanticAnalyzer(vector<Token*> _tokens, ErrorHandler* _error_handler);
	~SemanticAnalyzer();

private:
	int current_token_position;
	Token* current_token;

	vector<Token*> tokens;
	ErrorHandler* error_handler;

	map<string, Type> variables;
	map<DataType, Type> available_types = new map<>({ dtInt, new IntegerType() },
													{ dtReal, new RealType() },
													{ dtString, new StringType() },
													{ dtBool, new BoolType() },
													{ dtChar, new CharType() } );

	void next_token();
	// приводимо ли получ значение к типу переменной
	// 
	Type derive(Type left, Type right);

	bool accept(TokenType token_type, bool is_necessarily = false);
	bool accept(OperatorType operator_type, bool is_necessarily = false);
	bool accept(vector<OperatorType> operator_types, bool is_necessarily = false);

	void program();

	void block();

	void vars_section();
	void operators_section();

	bool single_var_definition();

	void type();

	void operator_();
	bool simple_operator();
	bool var();
	void expression();
	bool relation_operation();
	void simple_expression();
	bool additive_operation();
	void term();
	bool multiplicative_operation();
	void factor();
	bool sign();
	void complex_operator();
	void neccessary_compound_operator();
	bool compound_operator();
	bool if_operator();
	bool while_operator();
};