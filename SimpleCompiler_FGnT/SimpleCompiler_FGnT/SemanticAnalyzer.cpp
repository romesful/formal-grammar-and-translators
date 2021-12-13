#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(vector<Token*> _tokens, ErrorHandler* _error_handler)
{
	tokens = _tokens;
	error_handler = _error_handler;
}

SemanticAnalyzer::~SemanticAnalyzer()
{
	delete error_handler;
}

void SemanticAnalyzer::next_token()
{
	if (current_token_position == tokens.size())
		return;

	current_token = tokens[current_token_position];
	current_token_position++;
}

Type SemanticAnalyzer::derive(Type left, Type right)
{
	return Type();
}
