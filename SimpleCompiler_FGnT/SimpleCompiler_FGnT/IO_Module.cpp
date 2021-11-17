#include "IO_Module.h"

IO_Module::IO_Module(const string& filename_input, const string& filename_output)
{
	try
	{
		input_stream.open(filename_input);
	}
	catch (exception e)
	{
		cout << "Error opening file for INPUT: " << e.what() << endl;
		return;
	}

	try
	{
		output_stream.open(filename_output);
	}
	catch (exception e)
	{
		cout << "Error opening file for OUTPUT: " << e.what() << endl;
		return;
	}
}

char IO_Module::get_next_char()
{
	char c = (char)input_stream.get();
	return c;
}

void IO_Module::write_to_output(const string& info)
{

}
