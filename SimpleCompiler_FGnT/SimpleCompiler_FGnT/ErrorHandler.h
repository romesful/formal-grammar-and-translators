#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "Error.h"

using namespace std;

class ErrorHandler
{
private:
	vector<Error*> errors;
public:
	ErrorHandler() {};
	void add_error(string& info, int position)
	{
		errors.push_back(new Error(info, position));
	}

	void write_info(const string& filename_input, const string& filename_output);

	~ErrorHandler()
	{
		for (auto e : errors)
			delete e;
	}
};

#include "ErrorHandler.cpp"