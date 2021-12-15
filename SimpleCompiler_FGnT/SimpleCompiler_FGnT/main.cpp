#include <iostream>
#include <string>
#include "ErrorHandler.h"
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "SemanticAnalyzer.h"

using namespace std;

const string PATH_INPUT_FILE = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\input.txt";
const string PATH_OUTPUT_FILE = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\output.txt";

int main()
{
	setlocale(LC_ALL, "rus");
	setlocale(LC_NUMERIC, "eng"); // влияет на stod

	ErrorHandler* error_handler = new ErrorHandler();
	LexicalAnalyzer la(PATH_INPUT_FILE, PATH_OUTPUT_FILE, error_handler);
	la.check();
	auto tokens = la.get_tokens();
	SyntaxAnalyzer sa(tokens, error_handler);
	cout << endl;
	cout << sa.check();
	cout << endl;
	
	SemanticAnalyzer se_a(tokens, error_handler);

	se_a.check();

 	error_handler->write_info(PATH_INPUT_FILE, PATH_OUTPUT_FILE);
}

