#pragma once
#include <unordered_map>
#include <map>
#include "Token.h"

const map<string, OperatorType> OperatorKeyWords = {
	{"if", otIf},
	{"do", otDo},
	{"of", otOf},
	{"or", otOr},
	{"in", otIn},
	{"to", otTo},

	{"end", otEnd},
	{"var", otVar},
	{"div", otDiv},
	{"and", otAnd},
	{"not", otNot},
	{"for", otFor},
	{"mod", otMod},
	{"nil", otNil},
	{"set", otSet},

	{"then", otThen},
	{"else", otElse},
	{"case", otCase},
	{"file", otFile},
	{"goto", otGoto},
	{"type", otType},
	{"with", otWith},

	{"begin", otBegin},
	{"while", otWhile},
	{"array", otArray},
	{"const", otConst},
	{"label", otLabel},
	{"until", otUntil},

	{"program", otProgram},
	{"function", otFunction},
	{"procedure", otProcedure},

	{"integer", otInteger},
	{"bool", otBool},
	{"real", otReal},
	{"string", otString},
	{"char", otChar}
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
	{"]", otRightBracket},
	{"<>", otLessGreater}
};

const map<OperatorType, string> KeyWordByOperator = {
	{otIf, "if"},
	{otDo, "do"},
	{otOf, "of"},
	{otOr, "or"},
	{otIn, "in"},
	{otTo, "to"},
	{otEnd, "end"},
	{otVar, "var"},
	{otDiv, "div"},
	{otAnd, "and"},
	{otNot, "not"},
	{otFor, "for"},
	{otMod, "mod"},
	{otNil, "nil"},
	{otSet, "set"},
	{otThen, "then"},
	{otElse, "else"},
	{otCase, "case"},
	{otFile, "file"},
	{otGoto, "goto"},
	{otType, "type"},
	{otWith, "with"},
	{otBegin, "begin"},
	{otWhile, "while"},
	{otArray, "array"},
	{otConst, "const"},
	{otLabel, "label"},
	{otUntil, "until"},
	{otProgram, "program"},
	{otFunction, "function"},
	{otProcedure, "procedure"},
	{otInteger, "integer"},
	{otBool, "bool"},
	{otReal, "real"},
	{otString, "string"},
	{otChar, "char"},
	{otPlus, "+"},
	{otMinus, "-"},
	{otSlash, "/"},
	{otStar, "*"},
	{otDot, "."},
	{otDots, ".."},
	{otEqual, "="},
	{otLess, "<"},
	{otGreater, ">"},
	{otAssign, ":="},
	{otComma, ","},
	{otGreaterEqual, ">="},
	{otLessEqual, "<="},
	{otSemiColon, ";"},
	{otColon, ":"},
	{otLeftParenthesis, "("},
	{otRightParenthesis, ")"},
	{otLeftBracket, "["},
	{otRightBracket, "]"},
	{otLessGreater, "<>"}
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