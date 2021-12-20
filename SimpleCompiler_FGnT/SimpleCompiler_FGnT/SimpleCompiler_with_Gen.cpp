#include "pch.h"
#include "ErrorHandler.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"
#include "Translator.h"
#include <iostream>

using namespace System;

const string PATH_INPUT_FILE = "C:\\Users\\ilyar\\Source\\Repos\\\SimpleCompiler_with_Gen\\\SimpleCompiler_with_Gen\\files\\input.txt";

int main(cli::array<String ^> ^args)
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

	if (error_handler->get_errors_count() != 0)
	{
		io->write_errors(error_handler);
		return 0;
	}

	Translator^ translator = gcnew Translator(tokens);

    return 0;
}
