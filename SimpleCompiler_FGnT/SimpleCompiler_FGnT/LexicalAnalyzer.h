#pragma once

#include "IO_Module.h"
#include "HashTablesOfLexems.h"
#include "ErrorHandler.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class LexicalAnalyzer
{
public:
	IO_Module* io;
	ErrorHandler* error_handler;

	LexicalAnalyzer(IO_Module* _io, ErrorHandler* _error_handler);

	bool check();
	vector<Token*> get_tokens();

	~LexicalAnalyzer();
private:
	Token* get_token();
	int get_current_position();

	vector<Token*> tokens;
	int position = 0;
	char c;
};

#include "LexicalAnalyzer.cpp"