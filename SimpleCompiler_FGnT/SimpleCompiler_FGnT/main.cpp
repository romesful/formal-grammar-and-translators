#include <iostream>
#include "LexicalAnalyzer.h"

using namespace std;

const string PATH_INPUT_FILE  = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\input.txt";
const string PATH_OUTPUT_FILE = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\output.txt";

int main()
{
	setlocale(LC_ALL, "rus");
	setlocale(LC_NUMERIC, "eng"); // влияет на stod

	/*
	IO_Module io(PATH_INPUT_FILE, PATH_OUTPUT_FILE);
	char c = ' ';
	do
	{
		cout << c << endl;
		c = io->get_next_char();
	} while (c != EOF);*/
	LexicalAnalyzer la(PATH_INPUT_FILE, PATH_OUTPUT_FILE);
	la.parse_tokens();
	IdentificatorToken* it;
	OperatorToken* ot;

	for (auto t : la.tokens)
	{
		switch (t->token_type)
		{
		case ttIdentificator:
			it = (IdentificatorToken*)t;
			cout << "Идентификатор: " << it->name << endl;
			break;
		case ttOperator:
			ot = (OperatorToken*)t;
			cout << "Оператор: " << ot->operator_type << endl;
			break;
		case ttConst:
			if (auto b = dynamic_cast<ConstToken<int>*>(t)) {
				cout << b->value << endl;
			}
			else if (auto b = dynamic_cast<ConstToken<double>*>(t)) {
				cout << b->value << endl;
			}
			break;
		}
	}
}