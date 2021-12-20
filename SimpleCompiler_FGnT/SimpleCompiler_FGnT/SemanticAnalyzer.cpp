#include "pch.h"
#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer(vector<Token*> _tokens, ErrorHandler* _error_handler)
{
	tokens = _tokens;
	error_handler = _error_handler;

	current_token_position = 0;
	next_token();

	available_types[dtInt] = new IntegerType();
	available_types[dtReal] = new RealType();
	available_types[dtString] = new StringType();
	available_types[dtBool] = new BoolType();
	available_types[dtChar] = new CharType();
}

SemanticAnalyzer::~SemanticAnalyzer()
{
	delete error_handler;
}

bool SemanticAnalyzer::check()
{
	int errors_count = error_handler->get_errors_count();
	program();
	return error_handler->get_errors_count() == errors_count;
}

void SemanticAnalyzer::next_token()
{
	if (current_token_position == tokens.size())
		return;

	current_token = tokens[current_token_position];
	current_token_position++;

	if (current_token->token_type == ttOperator)
		get_last_position_of_operator[((OperatorToken*)current_token)->operator_type] = current_token->position;
}

Type* SemanticAnalyzer::derive(Type* left, Type* right, OperatorType last_operation, int position_for_error)
{
	Type* result = new Type();
	if (left->can_cast_to(right))
		result = right;
	if (right->can_cast_to(left))
		result = left;

	bool is_string = result->can_cast_to(available_types[dtString]);

	/*
	=|<>|<|<=|>=|>
	accept({ otEqual, otLessGreater, otLessEqual, otGreaterEqual, otGreater });
	+|-|or
	accept({ otPlus, otMinus, otOr });
	*|/|div|mod|and
	accept({ otStar, otSlash, otDiv, otMod, otAnd });
	*/

	string error_text;

	if (is_string)
	{
		// только +, =, <>
		if (last_operation != otPlus && last_operation != otEqual && last_operation != otLessGreater)
		{
			error_text = "Данную операцию нельзя применить к этим операндам";
			error_handler->add_error(error_text, position_for_error);
		}
	}
	else
	{
		if (left->can_cast_to(available_types[dtString])|| right->can_cast_to(available_types[dtString])) // хотя бы один операнд - строковый
		{
			error_text = "Данную операцию нельзя применить к этим операндам";
			error_handler->add_error(error_text, position_for_error);
		}
	}

	if (last_operation == otEqual || last_operation == otLessGreater || last_operation == otLessEqual ||
		last_operation == otGreaterEqual || last_operation == otGreater || last_operation == otLess ||
		last_operation == otOr || last_operation == otAnd)
		return available_types[dtBool];

	return result;
}

void SemanticAnalyzer::add_var(VarName name, Type* dt)
{
	if (variables.find(name) == variables.end())
		variables[name] = dt;
	else
	{
		string error_text = "Переменная с именем `" + name + "` уже была объявлена";
		error_handler->add_error(error_text, current_token->position);
	}
}

VarName SemanticAnalyzer::get_var_name_from_token(Token* token)
{
	if (token->token_type != ttIdentificator)
		return "";

	return ((IdentificatorToken*)token)->name;
}

Type* SemanticAnalyzer::get_type_from_const_token(Token* token)
{
	if (auto ct = dynamic_cast<ConstToken<int>*>(token)) {
		return available_types[ct->data_type];
	}
	else if (auto ct = dynamic_cast<ConstToken<double>*>(token)) {
		return available_types[ct->data_type];
	}
	else if (auto ct = dynamic_cast<ConstToken<string>*>(token)) {
		return available_types[ct->data_type];
	}
	else if (auto ct = dynamic_cast<ConstToken<char>*>(token)) {
		return available_types[ct->data_type];
	}
	else if (auto ct = dynamic_cast<ConstToken<bool>*>(token)) {
		return available_types[ct->data_type];
	}

	return new Type();
}

bool SemanticAnalyzer::accept(TType token_type)
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
		lastOp = operator_type;
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
				lastOp = operator_type;
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
	vector<VarName> variableNames;
	
	variableNames.push_back(get_var_name_from_token(current_token));
	if (!accept(ttIdentificator))
		return false;

	while (accept(otComma))
	{
		variableNames.push_back(get_var_name_from_token(current_token));
		accept(ttIdentificator);
	}

	accept(otColon);

	Type* varType = type();

	for (VarName name : variableNames)
	{
		add_var(name, varType);
	}

	return true;
}

Type* SemanticAnalyzer::type() // <тип>::=integer|real|string|char
{
	if (accept(otInteger))
		return available_types[dtInt];
	if (accept(otReal))
		return available_types[dtReal];
	if (accept(otString))
		return available_types[dtString];
	if (accept(otChar))
		return available_types[dtChar];
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
	int mem_position = current_token->position;
	VarName name = get_var_name_from_token(current_token);

	if (!accept(ttIdentificator))
		return false;

	accept(otAssign);
	Type* t = expression();

	if (variables.find(name) == variables.end())
	{
		string error_text = "Переменная не была объявлена";
		error_handler->add_error(error_text, mem_position);
	}
	else
	{
		if (!t->can_cast_to(variables[name]))
		{
			// TODO: вывод ошибки
			string error_text = "Вычисленное выражение имеет другой тип в отличие от переменной";
			error_handler->add_error(error_text, get_last_position_of_operator[otAssign]);
		}
	}

	return true;
}

//<выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
Type* SemanticAnalyzer::expression()
{
	Type *t1, *t2;
	t1 = simple_expression();
	if (relation_operation())
	{
		OperatorType last_operation = lastOp;
		int position = get_last_position_of_operator[last_operation];
		t2 = simple_expression();
		t1 = derive(t1, t2, last_operation, position);
	}

	return t1;
}

//<операция отношения>::= =|<>|<|<=|>=|>
bool SemanticAnalyzer::relation_operation()  // *
{
	return accept({ otEqual, otLessGreater, otLess, otLessEqual, otGreaterEqual, otGreater });
}

//<простое выражение>::=<слагаемое>{<аддитивная операция><слагаемое>}
Type* SemanticAnalyzer::simple_expression()
{
	Type* t1, * t2;
	t1 = term();
	while (additive_operation())
	{
		OperatorType last_operation = lastOp;
		int position = get_last_position_of_operator[last_operation];
		t2 = term();
		t1 = derive(t1, t2, last_operation, position);
	}
	return t1;
}

//<аддитивная операция>::= +|-|or
bool SemanticAnalyzer::additive_operation()  // *
{
	return accept({ otPlus, otMinus, otOr });
}

//<слагаемое>::=<множитель>{<мультипликативная операция><множитель>}
Type* SemanticAnalyzer::term()
{
	Type* t1, * t2;
	t1 = factor();
	while (multiplicative_operation())
	{
		OperatorType last_operation = lastOp;
		int position = get_last_position_of_operator[last_operation];
		t2 = factor();
		t1 = derive(t1, t2, last_operation, position);
	}

	return t1;
}

//<мультипликативная операция>::=*|/|div|mod|and
bool SemanticAnalyzer::multiplicative_operation()  // *
{
	return accept({ otStar, otSlash, otDiv, otMod, otAnd });
}

//<множитель>::=[<знак>]<переменная>|[<знак>]<константа>|[<знак>](<выражение>)|not <множитель>
Type* SemanticAnalyzer::factor()
{
	// TODO: сделать что-то со знаком...
	if (sign())
	{
		//...
	}

	VarName name = get_var_name_from_token(current_token);
	Type* const_type = get_type_from_const_token(current_token);
	int mem_position = current_token->position;
	if (accept(ttIdentificator))
	{
		if (variables.find(name) == variables.end())
		{
			string error_text = "Переменная не была объявлена";
			error_handler->add_error(error_text, mem_position);
			return new Type();
		}
		return variables[name];
	}
	else if (accept(ttConst))
	{
		return const_type;
	}
	else if (accept(otLeftParenthesis))
	{
		Type* t = expression();
		accept(otRightParenthesis);

		return t;
	}
	else if (accept(otNot))
	{
		Type* t = factor();

		if (!t->can_cast_to(available_types[dtBool]))
		{
			string error_text = "Выражение должно иметь тип Bool";
			error_handler->add_error(error_text, get_last_position_of_operator[otNot]);
			// TODO: ошибка
		}

		return available_types[dtBool];
	}

}

//<знак>::= +|-
bool SemanticAnalyzer::sign()  // *
{
	return accept({ otPlus, otMinus });
}

//<сложный оператор>::=<составной оператор>|<выбирающий оператор>|<оператор цикла>|<врайтлайн>
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
	else if (writeln())
	{
		//...
	}
	else if (readln())
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

	Type* t = expression();

	if (!t->can_cast_to(available_types[dtBool]))
	{
		string error_text = "Выражение должно иметь тип Bool";
		error_handler->add_error(error_text, current_token->position);
		// TODO: ошибка
	}

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

	Type* t = expression();
	if (!t->can_cast_to(available_types[dtBool]))
	{
		string error_text = "Выражение должно иметь тип Bool";
		error_handler->add_error(error_text, current_token->position);
		// TODO: ошибка
	}

	accept(otDo);
	operator_();

	return true;
}

bool SemanticAnalyzer::writeln()
{
	if (!accept(otWriteLn))
		return false;

	accept(otLeftParenthesis);
	expression();
	accept(otRightParenthesis);

	return true;
}

bool SemanticAnalyzer::readln()
{
	if (!accept(otReadLn))
		return false;

	accept(otLeftParenthesis);
	VarName name = get_var_name_from_token(current_token);
	Type* const_type = get_type_from_const_token(current_token);
	int mem_position = current_token->position;
	if (accept(ttIdentificator))
	{
		if (variables.find(name) == variables.end())
		{
			string error_text = "Переменная не была объявлена";
			error_handler->add_error(error_text, mem_position);
		}
	}
	accept(otRightParenthesis);

	return true;
}
