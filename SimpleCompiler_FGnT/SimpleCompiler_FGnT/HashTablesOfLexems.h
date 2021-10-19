#pragma once
#include <unordered_map>
#include <map>
#include "Token.h"

const map<string, OperatorType> OperatorKeyWords = {
	{"var", otVar},
	{"begin", otBegin},
	{"end", otEnd},
	{"if", otIf},
	{"then", otThen},
	{"else", otElse},
	{"while", otWhile}
};

const map<string, OperatorType> OperatorSymbols = {
	{"+", otPlus},
	{"-", otMinus},
	{"/", otSlash},
	{"*", otStar},
	{".", otDot},
	{"..", otDots},
	{"=", otEqual},
	{"<", otLess},
	{">", otGreater},
	{":=", otAssign},
	{",", otComma},
	{">=", otGreaterEqual},
	{"<=", otLessEqual},
	{";", otSemiColon},
	{":", otColon},
	{"(", otLeftParenthesis},
	{")", otRightParenthesis},
	{"[", otLeftBracket},
	{"]", otRightBracket}
};
/*

enum TokenType {
	ttIdentificator,
	ttOperator,
	ttConst
};

enum OperatorType {
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
	otWhile					// while
};

enum DataType {
	dtInt,
	dtReal,
	dtString,
	dtChar
};*/