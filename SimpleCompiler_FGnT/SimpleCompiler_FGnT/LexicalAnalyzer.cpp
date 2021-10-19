#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer(const string& filename_input, const string& filename_output)
{
	io = IO_Module(filename_input, filename_output);
}

void LexicalAnalyzer::parse_tokens()
{
	char c = ' ';
	do
	{
		switch (c)
		{
			case ' ': break;
		}
		c = io.get_next_char();
	} while (c != EOF);
}
