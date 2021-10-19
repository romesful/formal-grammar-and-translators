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
	void parse_tokens();
	~LexicalAnalyzer();
//private:
	vector<Token*> tokens;
};

#include "LexicalAnalyzer.cpp"