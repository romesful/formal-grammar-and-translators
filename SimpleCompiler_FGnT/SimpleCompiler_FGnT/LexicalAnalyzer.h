#include "IO_Module.h"
#pragma once
#include "HashTablesOfLexems.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

class LexicalAnalyzer
{
public:
	IO_Module* io;
	LexicalAnalyzer(const string& filename_input, const string& filename_output);
	Token* get_token();
	~LexicalAnalyzer();
private:
	char c;
};

#include "LexicalAnalyzer.cpp"