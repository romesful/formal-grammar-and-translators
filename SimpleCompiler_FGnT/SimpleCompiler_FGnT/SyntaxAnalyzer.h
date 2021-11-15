#pragma once

#include "LexicalAnalyzer.h"

class SyntaxAnalyzer
{
	LexicalAnalyzer* la;

	SyntaxAnalyzer(const string& filename_input, const string& filename_output);
	~SyntaxAnalyzer();

	bool check();
private:
	Token* current_token;
	bool syntax_analysis_result;

	void fail();

	bool accept(TokenType token_type);
	bool accept(OperatorType operator_type);

	void program();

	void block();

	void constants_section();
	void vars_section();
	void functions_section();
	void operators_section();

	bool constant_definition();
	bool single_var_definition();
	
	void type();
};