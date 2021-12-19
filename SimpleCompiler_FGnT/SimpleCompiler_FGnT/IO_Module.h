#pragma once

#include <iostream>
#include <fstream>
#include "ErrorHandler.h"

using namespace std;

enum OpenState {
	ByFile,
	ByString
};

class IO_Module
{
private:
	const string OUTPUT_POSTFIX = "ANALYZED";
	string path_to_file;
	fstream input_stream;
	string code;

	OpenState open_state;

	int position = -1;

	string change_filename(string& path);
public:
	IO_Module() {};
	IO_Module(const string& input);
	char get_next_char();
	void write_errors(ErrorHandler* error_handler);
	int get_current_position();
	~IO_Module();
};