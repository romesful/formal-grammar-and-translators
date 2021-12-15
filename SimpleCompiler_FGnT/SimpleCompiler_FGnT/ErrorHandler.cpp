#include "ErrorHandler.h"

void ErrorHandler::write_info(const string& filename_input, const string& filename_output)
{
	sort(errors.begin(), errors.end(), [](Error* a, Error* b) -> bool {return a->position < b->position; });
	ifstream fin(filename_input);
	ofstream fout(filename_output);

	int current_position = 0;
	int current_error = 0;

	do
	{
		int current_position_in_line = 0;

		vector<string> errors_info_in_line;
		vector<int> errors_positions_in_line;
		char c = ' ';
		c = fin.get();

		while (!fin.eof() && c != '\n')
		{
			
			if (current_error < errors.size() && errors[current_error]->position == current_position)
			{
				errors_info_in_line.push_back(errors[current_error]->info);
				errors_positions_in_line.push_back(current_position_in_line);
				current_error++;
			}

			current_position++;
			current_position_in_line++;
			if (c == '\t')
				current_position_in_line += 3;
/*

			current_position++;
			current_position_in_line++;
			if (c == '\t')
				current_position_in_line += 3;*/

			fout << c;
			cout << c << "-" << current_position-1 << endl;
			c = fin.get();
		}
		if (!fin.eof())
			fout << c;
		else
			fout << '\n';

		if (current_error < errors.size() && errors[current_error]->position == current_position)
		{
			errors_info_in_line.push_back(errors[current_error]->info);
			errors_positions_in_line.push_back(current_position_in_line);
			current_error++;
		}

		current_position++;
		current_position_in_line++;

		int e = errors_info_in_line.size();
		if (!e) continue;

		int len = current_position_in_line + 1;

		string first_line(len, ' ');
		for (int p : errors_positions_in_line)
			first_line[p] = '^';

		first_line += '\n';
		fout << first_line;

		for (int i = e-1; i>=0;i--)
		{
			string line(errors_positions_in_line[i], ' ');
			for (int j = 0; j < i; j++)
				line[errors_positions_in_line[j]] = '|';

			line += errors_info_in_line[i];
			line += '\n';
			
			fout << line;
		}
		fout << '\n';
	} while (!fin.eof());

	fin.close();
	fout.close();
}
