#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(const string& filename_input, const string& filename_output)
{
	io = new IO_Module(filename_input, filename_output);
	c = io->get_next_char();
}

Token* LexicalAnalyzer::get_token()
{
	while (c == ' ' || c == '\n' || c == '\r' || c == '\t')
		c = io->get_next_char();

	if (c == EOF)
	{
		return nullptr;
	}
	// Парсинг чисел
	else if (c == '-' || isdigit(c))
	{
		string lexem(1, c);
		c = io->get_next_char();

		if (!isdigit(c) && c == '-')
			return new OperatorToken(ttOperator, otMinus);

		if (isdigit(lexem[0]) && !isdigit(c))
			return new ConstToken<int>(ttConst, stoi(lexem), dtInt);

		lexem += c;

		if (isdigit(lexem[0]))
		{
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
				// если число слишком длинное - все плохо
				return new ConstToken<double>(ttConst, stod(lexem), dtReal);
			}
			else
			{
				return new ConstToken<int>(ttConst, stoi(lexem), dtInt);
			}
		}
		else
		{
			return new OperatorToken(ttOperator, otMinus);
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
			return new IdentificatorToken(ttIdentificator, lexem);
		}
		else
		{
			return new OperatorToken(ttOperator, OperatorKeyWords.at(lexem));
		}
	}
	// Парсинг символов
	else if (c == '\'')
	{
		char lexem = io->get_next_char();
		c = io->get_next_char();
		if (c != '\'') // ошибка
			continue;
		else
		{
			c = io->get_next_char();
			return new ConstToken<char>(ttConst, lexem, dtChar);
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

		return new ConstToken<string>(ttConst, lexem, dtString);
	}
	// Парсинг небуквенных операторов
	else
	{
		string lexem(1, c);
		OperatorType ot = otError;
		if (OperatorSymbols.find(lexem) != OperatorSymbols.end())
			ot = OperatorSymbols.at(lexem);

		/*
		.. | := | >= | <=
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

		return new OperatorToken(ttOperator, ot);

	}
}

LexicalAnalyzer::~LexicalAnalyzer()
{
	delete io;
}
