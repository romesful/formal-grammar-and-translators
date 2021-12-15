#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(vector<Token*> _tokens, ErrorHandler* _error_handler)
{
	tokens = _tokens;
	error_handler = _error_handler;

	current_token_position = 0;
	next_token();
}

SemanticAnalyzer::~SemanticAnalyzer()
{
	delete error_handler;
}

void SemanticAnalyzer::check()
{
	program();
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

void SemanticAnalyzer::add_var(varName name, Type dt)
{
	if (variables.find(name) == variables.end())
		variables[name] = dt;
	else
		return;
}

varName SemanticAnalyzer::get_var_name_from_token(Token* token)
{
	if (token->token_type != ttIdentificator)
		return "";

	return ((IdentificatorToken*)token)->name;
}

Type SemanticAnalyzer::get_type_from_const_token(Token* token)
{
	if (token->token_type != ttConst)
		return Type();

	return available_types[((ConstToken*)token)->data_type];
}

void SemanticAnalyzer::next_token()
{
	if (current_token_position == tokens.size())
		return;

	current_token = tokens[current_token_position];
	current_token_position++;
}

bool SemanticAnalyzer::accept(TokenType token_type)
{
	bool result = true;
	if (current_token->token_type != token_type)
		result = false;

	if (result)
		next_token();

	return result;
}

bool SemanticAnalyzer::accept(OperatorType operator_type)
{
	bool result = true;
	if (current_token->token_type != ttOperator)
		result = false;

	if (((OperatorToken*)current_token)->operator_type != operator_type)
		result = false;

	if (result)
	{
		lastOp = operatorType;
		next_token();
	}

	return result;
}

bool SemanticAnalyzer::accept(vector<OperatorType> operator_types)
{
	bool result = false;
	if (current_token->token_type == ttOperator)
	{
		OperatorType current_type = ((OperatorToken*)current_token)->operator_type;

		for (OperatorType operator_type : operator_types)
		{
			if (operator_type == current_type)
			{
				lastOp = operatorType;
				result = true;
				break;
			}
		}
	}

	if (result)
		next_token();

	return result;
}

void SemanticAnalyzer::program() // <программа>::=program <имя>(<имя файла>{,<имя файла>});<блок>.
{
	accept(otProgram);
	accept(ttIdentificator);
	accept(otSemiColon);

	block();

	accept(otDot);
}

void SemanticAnalyzer::block() // <блок>::=<раздел констант><раздел типов><раздел переменных><раздел процедур и функций><раздел операторов>
{
	vars_section();
	operators_section();
}

// ======== Раздел переменных ========
bool SemanticAnalyzer::single_var_definition() // <описание однотипных переменных>::=<имя>{,<имя>}:<тип>
{
	vector<varName> variableNames;
	
	variableNames.push_back(get_var_name_from_token(current_token));
	if (!accept(ttIdentificator))
		return false;

	while (accept(otComma))
	{
		variableNames.push_back(get_var_name_from_token(current_token));
		accept(ttIdentificator);
	}

	accept(otColon);

	Type varType = type();

	for (varName name : variableNames)
	{
		add_var(name, varType);
	}

	return true;
}

Type SemanticAnalyzer::type() // <тип>::=integer|real|string|char
{
	if (accept(otInteger))
		return available_types[otInteger];
	if (accept(otReal))
		return available_types[otReal];
	if (accept(otString))
		return available_types[otString];
	if (accept(otChar))
		return available_types[otChar];
}

void SemanticAnalyzer::vars_section() // <раздел переменных>::= var <описание однотипных переменных>;{<описание однотипных переменных>;} | <пусто>
{
	accept(otVar);

	single_var_definition();
	accept(otSemiColon);

	while (single_var_definition())
	{
		accept(otSemiColon);
	}
}

// ======== Раздел операторов ========
// <раздел операторов>::= <составной оператор>

void SemanticAnalyzer::operators_section()
{
	neccessary_compound_operator();
}

//<оператор>::=<простой оператор>|<сложный оператор>
void SemanticAnalyzer::operator_()
{
	if (!simple_operator())
		complex_operator();
}

//<простой оператор>::=<переменная>:=<выражение>
bool SemanticAnalyzer::simple_operator() // *
{
	varName name = get_var_name_from_token(current_token);
	if (!accept(ttIdentificator))
		return false;

	accept(otAssign);
	Type t = expression();

	return true;
}

//<выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
Type SemanticAnalyzer::expression()
{
	Type t;
	t = simple_expression();
	if (relation_operation())
		t = simple_expression();
}

//<операция отношения>::= =|<>|<|<=|>=|>
bool SemanticAnalyzer::relation_operation()  // *
{
	return accept({ otEqual, otLessGreater, otLessEqual, otGreaterEqual, otGreater });
}

//<простое выражение>::=<слагаемое>{<аддитивная операция><слагаемое>}
Type SemanticAnalyzer::simple_expression()
{
	Type t;
	t = term();
	while (additive_operation())
		t = term();
}

//<аддитивная операция>::= +|-|or
bool SemanticAnalyzer::additive_operation()  // *
{
	return accept({ otPlus, otMinus, otOr });
}

//<слагаемое>::=<множитель>{<мультипликативная операция><множитель>}
Type SemanticAnalyzer::term()
{
	Type t;
	t = factor();
	while (multiplicative_operation())
		t = factor();

	return t;
}

//<мультипликативная операция>::=*|/|div|mod|and
bool SemanticAnalyzer::multiplicative_operation()  // *
{
	return accept({ otStar, otSlash, otDiv, otMod, otAnd });
}

//<множитель>::=[<знак>]<переменная>|[<знак>]<константа>|[<знак>](<выражение>)|not <множитель>
Type SemanticAnalyzer::factor()
{
	if (sign())// можно представить как -1 * x либо 1 * x
	{
		//...
	}

	varName name = get_var_name_from_token(current_token);
	Type const_type = get_type_from_const_token(current_token);
	if (accept(ttIdentificator))
	{
		return variables[name];
	}
	else if (accept(ttConst))
	{
		return const_type;
	}
	else if (accept(otLeftParenthesis))
	{
		t = expression();
		accept(otRightParenthesis);

		return t;
	}
	else if (accept(otNot))
	{
		t = factor(); // возможна ошибка

		return available_types[dtBool];
	}

}

//<знак>::= +|-
bool SemanticAnalyzer::sign()  // *
{
	return accept({ otPlus, otMinus });
}

//<сложный оператор>::=<составной оператор>|<выбирающий оператор>|<оператор цикла>
void SemanticAnalyzer::complex_operator()
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
bool SemanticAnalyzer::compound_operator()  // *
{
	if (!accept(otBegin))
		return false;

	operator_();

	while (accept(otSemiColon))
	{
		operator_();
	}

	accept(otEnd);

	return true;
}

//<обязательный составной оператор>::= begin <оператор>{;<оператор>} end
void SemanticAnalyzer::neccessary_compound_operator()  // *
{
	accept(otBegin);

	operator_();

	while (accept(otSemiColon))
	{
		operator_();
	}

	accept(otEnd);
}

//<выбирающий оператор>::= if <выражение> then <оператор>|if <выражение> then <оператор> else <оператор>
bool SemanticAnalyzer::if_operator()  // *
{
	if (!accept(otIf))
		return false;

	Type t = expression();
	accept(otThen);
	operator_();

	if (accept(otElse))
		operator_();

	return true;
}

//<оператор цикла>::= while <выражение> do <оператор>
bool SemanticAnalyzer::while_operator()  // *
{
	if (!accept(otWhile))
		return false;

	expression();
	accept(otDo);
	operator_();

	return true;
}