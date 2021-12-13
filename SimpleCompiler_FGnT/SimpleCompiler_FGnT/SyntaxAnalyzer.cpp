#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(vector<Token*> _tokens, ErrorHandler* _error_handler)
{
	error_handler = _error_handler;
	tokens = _tokens;
	current_token_position = 0;
	next_token();
}

SyntaxAnalyzer::~SyntaxAnalyzer()
{
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

void SyntaxAnalyzer::next_token()
{
	if (current_token_position == tokens.size())
		return;

	current_token = tokens[current_token_position];
	current_token_position++;
}

bool SyntaxAnalyzer::accept(TokenType token_type, bool is_necessarily)
{
	bool result = true;
	if (current_token->token_type != token_type)
		result = false;
	if (result)
		next_token();
	else if (is_necessarily) // выводим ошибку
	{
		string error_text = "";
		switch (current_token->token_type)
		{
			case ttIdentificator:
				error_text = "Ожидалось имя идентификатора";
				break;
			case ttOperator:
				error_text = "Ожидалось имя оператора";
				break;
			case ttConst:
				error_text = "Ожидалась константа";
				break;
		}

		error_handler->add_error(error_text, current_token->position);

		syntax_analysis_result = false;

		next_token();
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
		next_token();
	}
	else if (is_necessarily) // ошибочка вышла
	{
		string error_text = "Ожидался оператор: " + KeyWordByOperator.find(operator_type)->second;

		error_handler->add_error(error_text, current_token->position);

		syntax_analysis_result = false; 

		next_token();
	}

	return result;
}

bool SyntaxAnalyzer::accept(vector<OperatorType> operator_types, bool is_necessarily)
{
	bool result = false;
	if (current_token->token_type == ttOperator)
	{
		OperatorType current_type = ((OperatorToken*)current_token)->operator_type;

		for (OperatorType operator_type : operator_types)
		{
			if (operator_type == current_type)
			{
				result = true;
				break;
			}
		}
	}

	if (result)
	{
		next_token();
	}
	else if (is_necessarily)
	{
		string error_text = "Ожидался один из операторов: ";

		for (OperatorType operator_type : operator_types)
			error_text += KeyWordByOperator.find(operator_type)->second + ", ";

		error_handler->add_error(error_text, current_token->position);

		syntax_analysis_result = false;

		next_token();
	}

	return result;
}

void SyntaxAnalyzer::program() // <программа>::=program <имя>(<имя файла>{,<имя файла>});<блок>.
{
	if (!accept(otProgram, true)) return;
	if (!accept(ttIdentificator, true)) return;
	if (!accept(otSemiColon, true)) return;

	block();

	if (!accept(otDot, true)) return;
}

void SyntaxAnalyzer::block() // <блок>::=<раздел констант><раздел типов><раздел переменных><раздел процедур и функций><раздел операторов>
{
	//constants_section();
	vars_section();
	//functions_section();
	operators_section();
}

// ======== Раздел переменных ========
bool SyntaxAnalyzer::single_var_definition() // <описание однотипных переменных>::=<имя>{,<имя>}:<тип>
{
	if (!accept(ttIdentificator))
		return false;

	while (accept(otComma))
	{
		if (!accept(ttIdentificator, true)) return false;
	}

	if (!accept(otColon, true)) return false;

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
	if (!accept(otSemiColon, true)) return;

	while (single_var_definition())
	{
		if (!accept(otSemiColon, true)) return;
	}
}

// ======== Раздел операторов ========
// <раздел операторов>::= <составной оператор>

void SyntaxAnalyzer::operators_section()
{
	neccessary_compound_operator();
}

//<оператор>::=<простой оператор>|<сложный оператор>
void SyntaxAnalyzer::operator_()
{
	if (!simple_operator())
		complex_operator();
}

//<простой оператор>::=<переменная>:=<выражение>
bool SyntaxAnalyzer::simple_operator() // *
{
	if (!var())
		return false;

	if (!accept(otAssign, true)) return false;
	expression();

	return true;
}

//<переменная>::=<имя>
bool SyntaxAnalyzer::var()  // *
{
	return accept(ttIdentificator);
}

//<выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
void SyntaxAnalyzer::expression()
{
	simple_expression();
	if (relation_operation())
		simple_expression();
}

//<операция отношения>::= =|<>|<|<=|>=|>
bool SyntaxAnalyzer::relation_operation()  // *
{
	return accept({ otEqual, otLessGreater, otLessEqual, otGreaterEqual, otGreater });
}

//<простое выражение>::=<слагаемое>{<аддитивная операция><слагаемое>}
void SyntaxAnalyzer::simple_expression()
{
	term();
	while (additive_operation())
		term();
}

//<аддитивная операция>::= +|-|or
bool SyntaxAnalyzer::additive_operation()  // *
{
	return accept({ otPlus, otMinus, otOr });
}

//<слагаемое>::=<множитель>{<мультипликативная операция><множитель>}
void SyntaxAnalyzer::term()
{
	factor();
	while (multiplicative_operation())
		factor();
}

//<мультипликативная операция>::=*|/|div|mod|and
bool SyntaxAnalyzer::multiplicative_operation()  // *
{
	return accept({ otStar, otSlash, otDiv, otMod, otAnd });
}

//<множитель>::=[<знак>]<переменная>|[<знак>]<константа>|[<знак>](<выражение>)|not <множитель>
void SyntaxAnalyzer::factor()
{
	if (sign())
	{
		//...
	}

	if (var())
	{
		//...
	}
	else if (accept(ttConst))
	{
		//...
	}
	else if (accept(otLeftParenthesis))
	{
		expression();
		accept(otRightParenthesis, true);
	}
	else if (accept(otNot))
	{
		factor();
	}
	else
	{
		string error_text = "Ожидалась переменная/константа/выражение";
		syntax_analysis_result = false;
		error_handler->add_error(error_text, current_token->position);
	}

}

//<знак>::= +|-
bool SyntaxAnalyzer::sign()  // *
{
	return accept({ otPlus, otMinus });
}

//<сложный оператор>::=<составной оператор>|<выбирающий оператор>|<оператор цикла>
void SyntaxAnalyzer::complex_operator()
{
	if (compound_operator())
	{
		//...
	}
	else if (if_operator())
	{
		//...
	}
	else if (while_operator())
	{
		//...
	}
}

//<составной оператор>::= begin <оператор>{;<оператор>} end
bool SyntaxAnalyzer::compound_operator()  // *
{
	if (!accept(otBegin))
		return false;

	operator_();

	while (accept(otSemiColon))
	{
		operator_();
	}

	if (!accept(otEnd, true)) return false;

	return true;
}

//<обязательный составной оператор>::= begin <оператор>{;<оператор>} end
void SyntaxAnalyzer::neccessary_compound_operator()  // *
{
	if (!accept(otBegin, true)) return;

	operator_();

	while (accept(otSemiColon))
	{
		operator_();
	}

	if (!accept(otEnd, true)) return;
}

//<выбирающий оператор>::= if <выражение> then <оператор>|if <выражение> then <оператор> else <оператор>
bool SyntaxAnalyzer::if_operator()  // *
{
	if (!accept(otIf))
		return false;

	expression();
	accept(otThen);
	operator_();

	if (accept(otElse))
		operator_();

	return true;
}

//<оператор цикла>::= while <выражение> do <оператор>
bool SyntaxAnalyzer::while_operator()  // *
{
	if (!accept(otWhile))
		return false;

	expression();
	if (!accept(otDo, true)) return false;
	operator_();

	return true;
}

/*


bool SyntaxAnalyzer::assignment_operator()
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
}*/

/*
<раздел операторов>::=<составной оператор>
<составной оператор>::= begin <оператор>{;<оператор>} end
<оператор>::=<простой оператор>|<сложный оператор>
<простой оператор>::=<переменная>:=<выражение>
<переменная>::=<имя>
<выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
<операция отношения>::= =|<>|<|<=|>=|>
<простое выражение>::=<слагаемое>{<аддитивная операция><слагаемое>}
<аддитивная операция>::= +|-|or
<слагаемое>::=<множитель>{<мультипликативная операция><множитель>}
<мультипликативная операция>::=*|/|div|mod|and
<множитель>::=[<знак>]<переменная>|<константа>|[<знак>](<выражение>)|not <множитель>
<сложный оператор>::=<составной оператор>|<выбирающий оператор>|<оператор цикла>|<оператор присоединения>
<выбирающий оператор>::= if <выражение> then <оператор>|if <выражение> then <оператор> else <оператор>
<оператор цикла>::= while <выражение> do <оператор>
*/
