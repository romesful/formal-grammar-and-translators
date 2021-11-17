#pragma once

#include "LexicalAnalyzer.h"

class SyntaxAnalyzer
{
public:
	LexicalAnalyzer* la;

	SyntaxAnalyzer(const string& filename_input, const string& filename_output);

	bool check();

	~SyntaxAnalyzer();
private:
	Token* current_token;
	bool syntax_analysis_result;

	void fail();

	bool accept(TokenType token_type, bool is_necessarily = false);
	bool accept(OperatorType operator_type, bool is_necessarily = false);
	bool accept(vector<OperatorType> operator_types, bool is_necessarily = false);

	void program();

	void block();

	void constants_section();
	void vars_section();
	void functions_section();
	void operators_section();

	bool constant_definition();
	bool single_var_definition();

	bool procedure_definition();
	bool function_definition();

	bool procedure_header();
	bool function_header();

	bool formal_parameters_section();
	
	void type();

	bool simple_operator();
	bool complex_operator();

	bool assignment_operator();
	bool var();

	bool expression();
	bool simple_expression();
	bool relation_operation();

	bool sign();
	bool additive_operation();
	bool multiplicative_operation();

	bool term();
	bool factor();
};

#include "SyntaxAnalyzer.cpp"