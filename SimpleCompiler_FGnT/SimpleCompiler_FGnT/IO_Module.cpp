#include "IO_Module.h"

IO_Module::IO_Module(const string& input)
{
	try
	{
		input_stream.open(input);
		path_to_file = input;
		open_state = ByFile;
	}
	catch (exception e)
	{
		code = input;
		open_state = ByString;
	}
}

IO_Module::~IO_Module()
{
	input_stream.close();
}

char IO_Module::get_next_char()
{
	position++;

	char c;
	if (input_stream.is_open())
	{
		c = (char)input_stream.get();
	}
	else
	{
		if (position < code.size())
			c = code[position];
		else
			c = EOF;
	}

	if (c == EOF)
		input_stream.close();
	return c;
}

int IO_Module::get_current_position()
{
	return position;
}


void IO_Module::write_errors(ErrorHandler* error_handler)
{
	vector<Error*> errors = error_handler->get_errors();
	IO_Module* io_helper;

	string result;

	if (open_state == ByFile)
		io_helper = new IO_Module(path_to_file);
	else
		io_helper = new IO_Module(code);

	int current_error = 0;
	char c;
	do
	{
		// Построчно будем искать позиция с ошибками
		int current_position_in_line = 0;
		ErrorHandler* error_handler_in_line = new ErrorHandler();

		string first_line;

		do
		{
			c = io_helper->get_next_char();

			// Если еще есть ошибки и на текущей позиции есть ошибка, тогда добавим ее
			if (current_error < errors.size() &&
				errors[current_error]->position == io_helper->get_current_position())
			{
				error_handler_in_line->add_error(errors[current_error]->info, current_position_in_line);
				current_error++;
			}

			current_position_in_line++;
			first_line += (c == '\t' ? '\t' : ' ');
			result += (c == EOF ? '\n' : c);

		} while (c != EOF && c != '\n');

		vector<Error*> errors_in_line = error_handler_in_line->get_errors();

		int errors_count_in_line = errors_in_line.size();
		if (errors_count_in_line == 0) continue;
		// Если в строке были ошибки то создадим первую строку типа:
		// ^             ^         ^
		// Указатели на позиции ошибок

		for (auto e : errors_in_line)
			first_line[e->position] = '^';

		first_line += '\n';
		result += first_line;

		for (int i = errors_count_in_line - 1; i >= 0; i--)
		{
			// Обрежем первую строку до нужной позиции, т.к. на предыдущих итерациях мы уже обработали некоторые ошибки
			string line = first_line.substr(0, errors_in_line[i]->position);
			// заменим на палочки, так красивее....
			for (int j = 0; j < line.size(); j++)
			{
				if (line[j] == '^')
					line[j] = '|';
			}

			// текущая ошибка всегда в конце строки, поэтому просто добавим описание ошибки в конец строки
			line += errors_in_line[i]->info;
			line += '\n';

			result += line;
		}

		result += '\n';

		delete error_handler_in_line;
	} while (c != EOF);

	if (open_state == ByFile)
	{
		string path_to_result = change_filename(path_to_file);
		ofstream fout(path_to_result);
		fout << result;
		fout.close();
	}
	else
	{
		cout << result;
	}

	delete io_helper;
}

string IO_Module::change_filename(string& path)
{
	// попытаемся найти где начинается extension
	string result;
	auto pos = path.rfind(".");
	if (pos == string::npos)
	{
		return path + OUTPUT_POSTFIX;
	}

	result = path.substr(0, pos) + "_" + OUTPUT_POSTFIX + path.substr(pos);

	return result;
}