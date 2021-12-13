#pragma once

#include <iostream>
#include <string>
using namespace std;

class Error
{
public:
	string info;
	int position;
	Error(string& _info, int _position)
	{
		info = _info;
		position = _position;
	}
};