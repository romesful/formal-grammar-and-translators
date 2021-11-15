#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(const string& filename_input, const string& filename_output)
{
	la = new LexicalAnalyzer(filename_input, filename_output);
	current_token = la->get_token();
}

SyntaxAnalyzer::~SyntaxAnalyzer()
{
	delete la;
	delete current_token;
}

bool SyntaxAnalyzer::check()
{
	syntax_analysis_result = true;

	program();

	return syntax_analysis_result;
}

void SyntaxAnalyzer::fail()
{
	syntax_analysis_result = false;
}

bool SyntaxAnalyzer::accept(TokenType token_type)
{
	if (current_token->token_type != token_type)
		return false;

	current_token = la->get_token();
	return true;
}

bool SyntaxAnalyzer::accept(OperatorType operator_type)
{
	if (((OperatorToken*)current_token)->token_type != token_type)
		return false;

	current_token = la->get_token();
	return true;
}

void SyntaxAnalyzer::program()
{
	if (!accept(otProgram))
		fail();

	if (!accept(ttIdentificator))
		fail();

	block();

	if (!accept(otDot))
		fail();
}

void SyntaxAnalyzer::block()
{
	constants_section();
	vars_section();
	functions_section();
	operators_section();
}

void SyntaxAnalyzer::constants_section()
{
	if (!accept(otConst))
		return;

	if (!constant_definition())
		fail();

	if (!accept(otSemiColon))
		fail();

	while (constant_definition())
	{
		if (!accept(otSemiColon))
			fail();
	}
}

bool SyntaxAnalyzer::constant_definition()
{
	if (!accept(ttIdentificator))
		return false;

	if (!accept(otEqual))
		fail();

	if (!accept(ttConst))
		fail();
	
	return true;
}

bool SyntaxAnalyzer::single_var_definition()
{
	if (!accept(ttIdentificator))
		return false;

	while (accept(otComma))
	{
		if (!accept(ttIdentificator))
			fail();
	}

	if (!accept(otColon))
		fail();

	type();

	return true;
}

void SyntaxAnalyzer::type()
{
	if (!accept(otInteger) &&
		!accept(otReal) &&
		!accept(otString) &&
		!accept(otChar))
		fail();
}

void SyntaxAnalyzer::vars_section()
{
	if (!accept(otVar))
		return;

	if (!single_var_definition())
		fail();

	while (single_var_definition())
	{
		if (!accept(otSemiColon))
			fail();
	}
}

void SyntaxAnalyzer::functions_section()
{
}

void SyntaxAnalyzer::operators_section()
{

}
