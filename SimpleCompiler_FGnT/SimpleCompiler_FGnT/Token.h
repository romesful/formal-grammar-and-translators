#pragma once
#include <iostream>

using namespace std;

enum TokenType {
	ttIdentificator,
	ttOperator,
	ttConst
};

enum OperatorType {
	otError,
	otPlus,					// +
	otMinus,				// -
	otSlash,				// / 
	otStar,					// *
	otDot,					// .
	otDots,					// ..
	otEqual,				// =
	otLess,					// <
	otGreater,				// >
	otAssign,				// :=
	otComma,				// ,
	otGreaterEqual,			// >=
	otLessEqual,			// <=
	otSemiColon,			// ;
	otColon,				// :
	otLeftParenthesis,		// (
	otRightParenthesis,		// )
	otLeftBracket,			// [
	otRightBracket,			// ]
	otVar,					// var
	otBegin,				// begin
	otEnd,					// end
	otIf,					// if
	otThen,					// then
	otElse,					// else
	otWhile,				// while
};

enum DataType {
	dtInt,
	dtReal,
	dtString,
	dtChar
};

class Token {
public:
	TokenType token_type;
	Token(TokenType token_type)
	{
		this->token_type = token_type;
	}
	virtual ~Token() = default;
};

class OperatorToken : public Token {
public:
	OperatorType operator_type;
	OperatorToken(TokenType token_type, OperatorType operator_type) : Token(token_type)
	{
		this->operator_type = operator_type;
	}
};

class IdentificatorToken : public Token {
public:
	string name;
	IdentificatorToken(TokenType token_type, string name) : Token(token_type)
	{
		this->name = name;
	}
};

template<typename T>
class ConstToken : public Token {
public:
	T value;
	DataType data_type;
	ConstToken(TokenType token_type, T value, DataType data_type) : Token(token_type)
	{
		this->value = value;
		this->data_type = data_type;
	}
};