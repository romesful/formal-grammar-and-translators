#pragma once

#include "LexicalAnalyzer.h"

class SyntaxAnalyzer
{
public:
	SyntaxAnalyzer(vector<Token*> _tokens, ErrorHandler* _error_handler);

	bool check();

	~SyntaxAnalyzer();
private:
	vector<Token*> tokens;
	ErrorHandler* error_handler;

	int current_token_position;
	Token* current_token;

	void next_token();

	bool accept(TType token_type, bool is_necessarily = false);
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
	bool writeln();
	bool readln();
};