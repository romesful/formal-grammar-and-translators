#include <iostream>
#include <string>
#include "ErrorHandler.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"

using namespace std;

const string PATH_INPUT_FILE = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\input.txt";

int main()
{
	setlocale(LC_ALL, "rus");
	setlocale(LC_NUMERIC, "eng");

	ErrorHandler* error_handler = new ErrorHandler();
	IO_Module* io = new IO_Module(PATH_INPUT_FILE);

	bool check_result = false;

	LexicalAnalyzer la(io, error_handler);
	check_result = la.check();
	if (!check_result)
	{
		cout << "Найдены ошибки во время лексического анализа" << endl;
	}

	// Спарсенные токены во время лексического анализа
	auto tokens = la.get_tokens();
	SyntaxAnalyzer sa(tokens, error_handler);
	check_result = sa.check();
	if (!check_result)
	{
		cout << "Найдены ошибки во время синтаксического анализа" << endl;
	}

	SemanticAnalyzer se_a(tokens, error_handler);
	check_result = se_a.check();
	if (!check_result)
	{
		cout << "Найдены ошибки во время семантического анализа" << endl;
	}

	io->write_errors(error_handler);
}

