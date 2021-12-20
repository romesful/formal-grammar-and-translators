#include "pch.h"
#include "ErrorHandler.h"

void ErrorHandler::add_error(string& info, int position)
{
	errors.push_back(new Error(info, position));
}

vector<Error*> ErrorHandler::get_errors()
{
	sort(errors.begin(), errors.end(), [](Error* a, Error* b) -> bool {return a->position < b->position; });
	return errors;
}

int ErrorHandler::get_errors_count()
{
	return errors.size();
}

ErrorHandler::~ErrorHandler()
{
	for (auto e : errors)
		delete e;
}
