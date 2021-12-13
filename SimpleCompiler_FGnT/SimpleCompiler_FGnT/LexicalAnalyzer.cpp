#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(const string& filename_input, const string& filename_output, ErrorHandler* _error_handler)
{
	error_handler = _error_handler;

	io = new IO_Module(filename_input, filename_output);
	c = io->get_next_char();
}

Token* LexicalAnalyzer::get_token()
{
	position = io->get_current_position();

	while (c == ' ' || c == '\n' || c == '\r' || c == '\t')
		c = io->get_next_char();

	if (c == EOF)
	{
		return nullptr;
	}
	// Парсинг чисел
	else if (isdigit(c))
	{
		string lexem(1, c);

		c = io->get_next_char();
		while (isdigit(c))
		{
			lexem += c;
			c = io->get_next_char();
		}

		// если число слишком длинное - все плохо

		if (c == '.') // типа real?
		{
			lexem += c;
			c = io->get_next_char();
			while (isdigit(c))
			{
				lexem += c;
				c = io->get_next_char();
			}

			return new ConstToken<double>(ttConst, stod(lexem), dtReal, position);
		}
		else
		{
			return new ConstToken<int>(ttConst, stoi(lexem), dtInt, position);
		}
	}
	// Парсинг идентификаторов/операторов
	else if (isalpha(c))
	{
		string lexem(1, c);
		c = io->get_next_char();

		while (isdigit(c) || isalpha(c))
		{
			lexem += c;
			c = io->get_next_char();
		}

		if (OperatorKeyWords.find(lexem) == OperatorKeyWords.end())
		{
			return new IdentificatorToken(ttIdentificator, lexem, position);
		}
		else
		{
			return new OperatorToken(ttOperator, OperatorKeyWords.at(lexem), position);
		}
	}
	// Парсинг символов
	else if (c == '\'')
	{
		char lexem = io->get_next_char();
		c = io->get_next_char();
		if (c != '\'') // ошибка
			throw;
		else
		{
			c = io->get_next_char();
			return new ConstToken<char>(ttConst, lexem, dtChar, position);
		}
	}
	// Парсинг строк
	else if (c == '"')
	{
		string lexem = "";
		c = io->get_next_char();
		// если закрытия строки не будет - ошибку как-то
		while (c != '"')
		{
			lexem += c;
			c = io->get_next_char();
		}

		return new ConstToken<string>(ttConst, lexem, dtString, position);
	}
	// Парсинг небуквенных операторов
	else
	{
		string lexem(1, c);
		OperatorType ot = otError;
		if (OperatorSymbols.find(lexem) != OperatorSymbols.end())
			ot = OperatorSymbols.at(lexem);

		/*
		.. | := | >= | <= | <>
		*/
		switch (ot)
		{
		case otDot:
			c = io->get_next_char();
			if (c == '.')
			{
				ot = otDots;
				c = io->get_next_char();
			}
			break;
		case otColon:
			c = io->get_next_char();
			if (c == '=')
			{
				ot = otAssign;
				c = io->get_next_char();
			}
			break;
		case otLess:
			c = io->get_next_char();
			if (c == '=')
			{
				ot = otLessEqual;
				c = io->get_next_char();
			}
			else if (c == '>')
			{
				ot = otLessGreater;
				c = io->get_next_char();
			}
			break;
		case otGreater:
			c = io->get_next_char();
			if (c == '=')
			{
				ot = otGreaterEqual;
				c = io->get_next_char();
			}
			break;
		default:
			c = io->get_next_char();
		}

		if (ot == otError)
			return new Token(ttUndefined, position);

		return new OperatorToken(ttOperator, ot, position);

	}
}

int LexicalAnalyzer::get_current_position()
{
	return position;
}

bool LexicalAnalyzer::check()
{
	Token* new_token = get_token();
	do
	{
		tokens.push_back(new_token);
		new_token = get_token();
	} while (new_token != nullptr);

	tokens.push_back(new Token(ttUndefined, io->get_current_position()));

	return true;
}

vector<Token*> LexicalAnalyzer::get_tokens()
{
	return tokens;
}

LexicalAnalyzer::~LexicalAnalyzer()
{
	delete io;
}
