#pragma once
#include <vector>
#include <string>
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
	void add_error(string& info, int position);
	vector<Error*> get_errors();
	int get_errors_count();
	~ErrorHandler();
};