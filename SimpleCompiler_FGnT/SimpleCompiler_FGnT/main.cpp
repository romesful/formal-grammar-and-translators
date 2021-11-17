#include <iostream>
#include "SyntaxAnalyzer.h"

using namespace std;

const string PATH_INPUT_FILE  = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\input.txt";
const string PATH_OUTPUT_FILE = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\output.txt";

int main()
{
	setlocale(LC_ALL, "rus");
	setlocale(LC_NUMERIC, "eng"); // влияет на stod

	SyntaxAnalyzer sa(PATH_INPUT_FILE, PATH_OUTPUT_FILE);

	cout << sa.check();

	/*LexicalAnalyzer la(PATH_INPUT_FILE, PATH_OUTPUT_FILE);
	IdentificatorToken* it;
	OperatorToken* ot;

	Token* t;

	while (t = la.get_token())
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
			cout << "Константа: ";
			if (auto b = dynamic_cast<ConstToken<int>*>(t)) {
				cout << b->value << endl;
			}
			else if (auto b = dynamic_cast<ConstToken<double>*>(t)) {
				cout << b->value << endl;
			}
			break;
		}
	}*/
 }