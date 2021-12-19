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

	if (current_token->token_type == ttOperator)
		get_last_position_of_operator[((OperatorToken*)current_token)->operator_type] = current_token->position;
}

Type* SemanticAnalyzer::derive(Type* left, Type* right)
{
	// ������� ��� �������� ���?
	return new Type();
}

void SemanticAnalyzer::add_var(VarName name, Type* dt)
{
	if (variables.find(name) == variables.end())
		variables[name] = dt;
	else
	{
		string error_text = "���������� � ������ `" + name + "` ��� ���� ���������";
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

void SemanticAnalyzer::program() // <���������>::=program <���>(<��� �����>{,<��� �����>});<����>.
{
	accept(otProgram);
	accept(ttIdentificator);
	accept(otSemiColon);

	block();

	accept(otDot);
}

void SemanticAnalyzer::block() // <����>::=<������ ��������><������ �����><������ ����������><������ �������� � �������><������ ����������>
{
	vars_section();
	operators_section();
}

// ======== ������ ���������� ========
bool SemanticAnalyzer::single_var_definition() // <�������� ���������� ����������>::=<���>{,<���>}:<���>
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

Type* SemanticAnalyzer::type() // <���>::=integer|real|string|char
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

void SemanticAnalyzer::vars_section() // <������ ����������>::= var <�������� ���������� ����������>;{<�������� ���������� ����������>;} | <�����>
{
	accept(otVar);

	single_var_definition();
	accept(otSemiColon);

	while (single_var_definition())
	{
		accept(otSemiColon);
	}
}

// ======== ������ ���������� ========
// <������ ����������>::= <��������� ��������>

void SemanticAnalyzer::operators_section()
{
	neccessary_compound_operator();
}

//<��������>::=<������� ��������>|<������� ��������>
void SemanticAnalyzer::operator_()
{
	if (!simple_operator())
		complex_operator();
}

//<������� ��������>::=<����������>:=<���������>
bool SemanticAnalyzer::simple_operator() // *
{
	VarName name = get_var_name_from_token(current_token);
	if (!accept(ttIdentificator))
		return false;

	accept(otAssign);
	Type* t = expression();

	if (!t->can_cast_to(variables[name]))
	{
		// TODO: ����� ������
		string error_text = "����������� ��������� ����� ������ ��� � ������� �� ����������";
		error_handler->add_error(error_text, current_token->position);
	}

	return true;
}

//<���������>::=<������� ���������>|<������� ���������><�������� ���������><������� ���������>
Type* SemanticAnalyzer::expression()
{
	Type *t1, *t2;
	t1 = simple_expression();
	if (relation_operation())
	{
		t2 = simple_expression();
		t1 = derive(t1, t2);
	}

	return t1;
}

//<�������� ���������>::= =|<>|<|<=|>=|>
bool SemanticAnalyzer::relation_operation()  // *
{
	return accept({ otEqual, otLessGreater, otLessEqual, otGreaterEqual, otGreater });
}

//<������� ���������>::=<���������>{<���������� ��������><���������>}
Type* SemanticAnalyzer::simple_expression()
{
	Type* t1, * t2;
	t1 = term();
	while (additive_operation())
	{
		t2 = term();
		t1 = derive(t1, t2);
	}
	return t1;
}

//<���������� ��������>::= +|-|or
bool SemanticAnalyzer::additive_operation()  // *
{
	return accept({ otPlus, otMinus, otOr });
}

//<���������>::=<���������>{<����������������� ��������><���������>}
Type* SemanticAnalyzer::term()
{
	Type* t1, * t2;
	t1 = factor();
	while (multiplicative_operation())
	{
		t2 = factor();
		t1 = derive(t1, t2);
	}

	return t1;
}

//<����������������� ��������>::=*|/|div|mod|and
bool SemanticAnalyzer::multiplicative_operation()  // *
{
	return accept({ otStar, otSlash, otDiv, otMod, otAnd });
}

//<���������>::=[<����>]<����������>|[<����>]<���������>|[<����>](<���������>)|not <���������>
Type* SemanticAnalyzer::factor()
{
	if (sign())// ����� ����������� ��� -1 * x ���� 1 * x
	{
		//...
	}

	VarName name = get_var_name_from_token(current_token);
	Type* const_type = get_type_from_const_token(current_token);
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
		Type* t = expression();
		accept(otRightParenthesis);

		return t;
	}
	else if (accept(otNot))
	{
		Type* t = factor();

		if (!t->can_cast_to(available_types[dtBool]))
		{
			string error_text = "��������� ������ ����� ��� Bool";
			error_handler->add_error(error_text, current_token->position);
			// TODO: ������
		}

		return available_types[dtBool];
	}

}

//<����>::= +|-
bool SemanticAnalyzer::sign()  // *
{
	return accept({ otPlus, otMinus });
}

//<������� ��������>::=<��������� ��������>|<���������� ��������>|<�������� �����>
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

//<��������� ��������>::= begin <��������>{;<��������>} end
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

//<������������ ��������� ��������>::= begin <��������>{;<��������>} end
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

//<���������� ��������>::= if <���������> then <��������>|if <���������> then <��������> else <��������>
bool SemanticAnalyzer::if_operator()  // *
{
	if (!accept(otIf))
		return false;

	Type* t = expression();

	if (!t->can_cast_to(available_types[dtBool]))
	{
		string error_text = "��������� ������ ����� ��� Bool";
		error_handler->add_error(error_text, current_token->position);
		// TODO: ������
	}

	accept(otThen);
	operator_();

	if (accept(otElse))
		operator_();

	return true;
}

//<�������� �����>::= while <���������> do <��������>
bool SemanticAnalyzer::while_operator()  // *
{
	if (!accept(otWhile))
		return false;

	Type* t = expression();
	if (!t->can_cast_to(available_types[dtBool]))
	{
		string error_text = "��������� ������ ����� ��� Bool";
		error_handler->add_error(error_text, current_token->position);
		// TODO: ������
	}

	accept(otDo);
	operator_();

	return true;
}