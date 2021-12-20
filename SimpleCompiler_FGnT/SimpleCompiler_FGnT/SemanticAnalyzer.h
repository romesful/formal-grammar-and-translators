#pragma once
#include "ErrorHandler.h"
#include "Token.h"
#include "Type.h"
#include <vector>
#include <map>
#include <iostream>
#include <string>

using namespace std;

typedef string VarName;

class SemanticAnalyzer
{
public:
	SemanticAnalyzer(vector<Token*> _tokens, ErrorHandler* _error_handler);
	~SemanticAnalyzer();

	bool check();

private:
	int current_token_position;
	Token* current_token;

	vector<Token*> tokens;
	ErrorHandler* error_handler;

	map<VarName, Type*> variables;
	map<DataType, Type*> available_types;
	map<OperatorType, int> get_last_position_of_operator;
	OperatorType lastOp;

	void next_token();

	Type* derive(Type* left, Type* right, OperatorType last_operation, int position_for_error);

	void add_var(VarName name, Type* dt);
	VarName get_var_name_from_token(Token* token);
	Type* get_type_from_const_token(Token* token);

	bool accept(TType token_type);
	bool accept(OperatorType operator_type);
	bool accept(vector<OperatorType> operator_types);

	void program();

	void block();

	void vars_section();
	void operators_section();

	bool single_var_definition();

	Type* type();

	void operator_();
	bool simple_operator();
	Type* expression();
	bool relation_operation();
	Type* simple_expression();
	bool additive_operation();
	Type* term();
	bool multiplicative_operation();
	Type* factor();
	bool sign();
	void complex_operator();
	void neccessary_compound_operator();
	bool compound_operator();
	bool if_operator();
	bool while_operator();
	bool writeln();
	bool readln();
};