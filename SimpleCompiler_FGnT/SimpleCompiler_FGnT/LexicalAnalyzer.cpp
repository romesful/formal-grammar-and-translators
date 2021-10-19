#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(const string& filename_input, const string& filename_output)
{
	io = new IO_Module(filename_input, filename_output);
}

void LexicalAnalyzer::parse_tokens()
{
	char c = ' ';
	do
	{
		if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
			c = io->get_next_char();
		else if (c == '-')
		{
			string lexem = "-";
			c = io->get_next_char();
			if ('1' <= c && c <= '9')
			{
				lexem += c;
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
					tokens.push_back(new ConstToken<double>(ttConst, stod(lexem), dtReal));
				}
				else
				{
					tokens.push_back(new ConstToken<int>(ttConst, stoi(lexem), dtInt));
				}
			}
			else
			{
				tokens.push_back(new OperatorToken(ttOperator, otMinus));
			}
		}
		else if (isdigit(c))
		{
			string lexem(1, c);
			c = io->get_next_char();

			if (lexem[0] == '0' && c == '.')
			{
				lexem += c;
				c = io->get_next_char();
				while (isdigit(c))
				{
					lexem += c;
					c = io->get_next_char();
				}
				// если число слишком длинное - все плохо
				tokens.push_back(new ConstToken<double>(ttConst, stod(lexem), dtReal));
			}
			else
			{
				while ('0' <= c && c <= '9')
				{
					lexem += c;
					c = io->get_next_char();
				}
				// если число слишком длинное - все плохо
				tokens.push_back(new ConstToken<int>(ttConst, stoi(lexem), dtInt));
			}
		}
		else if (isalpha(c))
		{
			string lexem(1,c);
			c = io->get_next_char();

			while (isdigit(c) || isalpha(c))
			{
				lexem += c;
				c = io->get_next_char();
			}

			if (OperatorKeyWords.find(lexem) == OperatorKeyWords.end())
			{
				tokens.push_back(new IdentificatorToken(ttIdentificator, lexem));
			}
			else
			{
				tokens.push_back(new OperatorToken(ttOperator, OperatorKeyWords.at(lexem)));
			}
		}
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

			tokens.push_back(new OperatorToken(ttOperator, ot));
		}
	} while (c != EOF);
}

LexicalAnalyzer::~LexicalAnalyzer()
{
	delete io;
	for (auto t : tokens)
	{
		delete t;
	}
	tokens.clear();
}
