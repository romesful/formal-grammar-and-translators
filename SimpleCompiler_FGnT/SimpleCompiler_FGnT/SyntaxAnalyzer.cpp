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

bool SyntaxAnalyzer::accept(TokenType token_type, bool is_necessarily)
{
	bool result = true;
	if (current_token->token_type != token_type)
		result = false;
	if (result)
		current_token = la->get_token();
	else if (is_necessarily) // выводим ошибку
	{
		syntax_analysis_result = false;
	}
	return result;
}

bool SyntaxAnalyzer::accept(OperatorType operator_type, bool is_necessarily)
{
	bool result = true;
	if (current_token->token_type != ttOperator)
		result = false;

	if (((OperatorToken*)current_token)->operator_type != operator_type)
		result = false;

	if (result)
	{
		current_token = la->get_token();
	}
	else if (is_necessarily)
	{
		syntax_analysis_result = false;
	}

	return result;
}

bool SyntaxAnalyzer::accept(vector<OperatorType> operator_types, bool is_necessarily)
{
	bool result = true;
	if (current_token->token_type != ttOperator)
		result = false;

	OperatorType current_type = ((OperatorToken*)current_token)->operator_type;

	for (OperatorType operator_type : operator_types)
	{
		if (operator_type == current_type)
		{
			result = true;
			break;
		}
	}

	if (result)
	{
		current_token = la->get_token();
	}
	else if (is_necessarily)
	{
		syntax_analysis_result = false;
	}

	return result;
}

void SyntaxAnalyzer::program() // <программа>::=program <имя>(<имя файла>{,<имя файла>});<блок>.
{
	accept(otProgram, true);
	accept(ttIdentificator, true);
	accept(otSemiColon, true);

	block();

	accept(otDot, true);
}

void SyntaxAnalyzer::block() // <блок>::=<раздел констант><раздел типов><раздел переменных><раздел процедур и функций><раздел операторов>
{
	constants_section();
	vars_section();
	functions_section();
	operators_section();
}

void SyntaxAnalyzer::constants_section() // <раздел констант>::=<пусто>|const <определение константы>;{<определение константы>;}
{
	if (!accept(otConst))
		return;

	constant_definition();

	accept(otSemiColon, true);

	while (constant_definition())
	{
		accept(otSemiColon, true);
	}
}

bool SyntaxAnalyzer::constant_definition()// <определение константы>::=<имя>=<константа>
{
	if (!accept(ttIdentificator))
		return false;

	accept(otEqual, true);
	accept(ttConst, true);

	return true;
}

bool SyntaxAnalyzer::single_var_definition() // <описание однотипных переменных>::=<имя>{,<имя>}:<тип>
{
	if (!accept(ttIdentificator))
		return false;

	while (accept(otComma))
	{
		accept(ttIdentificator, true);
	}

	accept(otColon, true);

	type();

	return true;
}

void SyntaxAnalyzer::type() // <тип>::=integer|real|string|char
{
	accept({ otInteger, otReal, otString, otChar }, true);
}

void SyntaxAnalyzer::vars_section() // <раздел переменных>::= var <описание однотипных переменных>;{<описание однотипных переменных>;} | <пусто>
{
	if (!accept(otVar))
		return;

	single_var_definition();
	accept(otSemiColon, true);

	while (single_var_definition())
	{
		accept(otSemiColon, true);
	}
}

bool SyntaxAnalyzer::procedure_definition() // <описание процедуры>::=<заголовок процедуры><блок>
{
	if (!procedure_header())
		return false;

	block();

	return true;
}

bool SyntaxAnalyzer::function_definition()
{
	if (!function_header())
		return false;

	block();

	return true;
}

bool SyntaxAnalyzer::procedure_header() // <заголовок процедуры>::= procedure <имя>;| procedure <имя>(<раздел формальных параметров>{; <раздел формальных параметров>});
{
	if (!accept(otProcedure))
	{
		return false;
	}

	accept(ttIdentificator, true);

	if (accept(otSemiColon))
	{
		return true;
	}
	else if (accept(otLeftParenthesis))
	{
		formal_parameters_section();

		while (accept(otSemiColon))
		{
			formal_parameters_section();
		}

		accept(otRightParenthesis, true);

		accept(otSemiColon, true);

		return true;
	}
	
	//ожидалось otSemiColon или otLeftParenthesis
	fail();
	return false;
}

bool SyntaxAnalyzer::function_header() // <заголовок функции>::= function <имя>:<тип результата>;| function <имя>(<раздел формальных параметров>{;<раздел формальных параметров>}):<тип результата>;
{
	if (!accept(otFunction))
		return false;
	
	accept(ttIdentificator, true);

	if (accept(otColon))
	{
		type();

		accept(otSemiColon, true);

		return true;
	}
	else if (accept(otLeftParenthesis))
	{
		formal_parameters_section();

		while (accept(otSemiColon))
		{
			formal_parameters_section();
		}

		accept(otRightParenthesis, true);

		accept(otSemiColon, true);

		return true;
	}

	// ожидалось otColon или otLeftParenthesis
	fail();

	return true;
}

bool SyntaxAnalyzer::formal_parameters_section() // <раздел формальных параметров>::=var <группа параметров>
{
	accept(otVar, true);

	single_var_definition();

	return true;
}

void SyntaxAnalyzer::functions_section() // <раздел процедур и функций>::={<описание процедуры или функции>;}
{
	while (procedure_definition() || function_definition())
	{
		accept(otSemiColon, true);
	}
}

void SyntaxAnalyzer::operators_section() // <оператор>::=<простой оператор>|<сложный оператор>
{
	if (simple_operator())
		return;
	
	complex_operator();
}


bool SyntaxAnalyzer::simple_operator() // <простой оператор>::=<оператор присваивания>|<оператор процедуры>|<оператор перехода>|<пустой оператор>
{
	return false;
}

bool SyntaxAnalyzer::complex_operator() // <сложный оператор>::=<составной оператор>|<выбирающий оператор>|<оператор цикла>|<оператор присоединения>
{
	return false;
}

bool SyntaxAnalyzer::assignment_operator() // <оператор присваивания>::=<переменная>:=<выражение>       <имя функции>::=<имя>
{
	if (!var())
		return false;

	accept(otColon, true);
	accept(otEqual, true);

	expression();

	return true;
}

bool SyntaxAnalyzer::var() // <переменная>::=<имя>|<имя>[<выражение>{,<выражение>}]
{
	if (!accept(ttIdentificator))
		return false;

	if (accept(otLeftBracket))
	{
		expression();

		while (accept(otComma))
		{
			expression();
		}
		
		accept(otRightBracket, true);
	}

	return true;
}

bool SyntaxAnalyzer::expression() // <выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
{
	simple_expression();

	if (relation_operation())
	{
		simple_expression();
	}

	return true;
}

bool SyntaxAnalyzer::simple_expression() // <простое выражение>::=<знак><слагаемое>{<аддитивная операция><слагаемое>}
{
	sign();

	term();

	while (additive_operation())
	{
		term();
	}

	return true;
}

bool SyntaxAnalyzer::relation_operation() // <операция отношения>::= =|<>|<|<=|>=|>|in
{
	if (!accept({ otEqual, otLessGreater, otLessEqual, otGreaterEqual, otGreater, otIn }))
		return false;

	return true;
}

bool SyntaxAnalyzer::sign() // + | -
{
	accept({ otPlus, otMinus }, true);

	return true;
}

bool SyntaxAnalyzer::additive_operation() // + | - | or
{
	if (!accept({ otPlus, otMinus, otOr }))
		return false;

	return true;
}

bool SyntaxAnalyzer::multiplicative_operation() // <мультипликативная операция>::=*|/|div|mod|and
{
	if (!accept({ otStar, otSlash, otDiv, otMod, otAnd }))
		return false;

	return true;
}

bool SyntaxAnalyzer::term() // <слагаемое>::=<множитель>{<мультипликативная операция><множитель>}
{
	factor();

	while (multiplicative_operation())
		factor();

	return true;
}

bool SyntaxAnalyzer::factor() // <множитель>::=<переменная>|<константа>|(<выражение>)|<обозначение функции>|<множество>|not <множитель>
{
	if (accept(ttIdentificator))
		return true;
	if (accept(ttConst))
		return true;
	if (accept(otLeftParenthesis))
	{
		expression();
		accept(otRightParenthesis, true);
		return true;
	}
	if (1) // множество
		return true;

	if (accept(otNot, true))
	{
		factor();
		return true;
	}

	return false;
}
