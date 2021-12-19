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

Type SemanticAnalyzer::type() // <���>::=integer|real|string|char
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
	varName name = get_var_name_from_token(current_token);
	if (!accept(ttIdentificator))
		return false;

	accept(otAssign);
	Type t = expression();

	return true;
}

//<���������>::=<������� ���������>|<������� ���������><�������� ���������><������� ���������>
Type SemanticAnalyzer::expression()
{
	Type t;
	t = simple_expression();
	if (relation_operation())
		t = simple_expression();
}

//<�������� ���������>::= =|<>|<|<=|>=|>
bool SemanticAnalyzer::relation_operation()  // *
{
	return accept({ otEqual, otLessGreater, otLessEqual, otGreaterEqual, otGreater });
}

//<������� ���������>::=<���������>{<���������� ��������><���������>}
Type SemanticAnalyzer::simple_expression()
{
	Type t;
	t = term();
	while (additive_operation())
		t = term();
}

//<���������� ��������>::= +|-|or
bool SemanticAnalyzer::additive_operation()  // *
{
	return accept({ otPlus, otMinus, otOr });
}

//<���������>::=<���������>{<����������������� ��������><���������>}
Type SemanticAnalyzer::term()
{
	Type t;
	t = factor();
	while (multiplicative_operation())
		t = factor();

	return t;
}

//<����������������� ��������>::=*|/|div|mod|and
bool SemanticAnalyzer::multiplicative_operation()  // *
{
	return accept({ otStar, otSlash, otDiv, otMod, otAnd });
}

//<���������>::=[<����>]<����������>|[<����>]<���������>|[<����>](<���������>)|not <���������>
Type SemanticAnalyzer::factor()
{
	if (sign())// ����� ����������� ��� -1 * x ���� 1 * x
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
		t = factor(); // �������� ������

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

	Type t = expression();
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

	expression();
	accept(otDo);
	operator_();

	return true;
}