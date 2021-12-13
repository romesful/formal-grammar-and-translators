#pragma once
#include <iostream>
#include <fstream>

using namespace std;

class IO_Module
{
private:
	fstream input_stream;
	fstream output_stream;
	int position = -1;
public:
	IO_Module(const string& filename_input, const string& filename_output);
	char get_next_char();
	void write_to_output(const string& info);
	int get_current_position();
	~IO_Module();
};

