#pragma once
#include <vector>
#include <string>
#pragma once

#include <fstream>
#include <algorithm>
#include "Error.h"

using namespace std;

class ErrorHandler
{
private:
	vector<Error*> errors;
public:
	ErrorHandler() {};
	inline void add_error(string& info, int position);
	inline vector<Error*> get_errors();
	inline int get_errors_count();
	inline ~ErrorHandler();
};

#include "ErrorHandler.cpp"