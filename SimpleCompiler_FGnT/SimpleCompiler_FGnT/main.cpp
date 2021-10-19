#include <iostream>
#include "LexicalAnalyzer.h"

using namespace std;

const string PATH_INPUT_FILE  = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\input.txt";
const string PATH_OUTPUT_FILE = "C:\\Users\\ilyar\\Source\\Repos\\simple_compiler\\SimpleCompiler_FGnT\\SimpleCompiler_FGnT\\files\\output.txt";

int main()
{
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
}