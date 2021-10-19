#include "IO_Module.h"
#include "Token.h"
#include <vector>
#include <unordered_map>

class LexicalAnalyzer {
public:
	IO_Module io;
	LexicalAnalyzer(const string& filename_input, const string& filename_output);
	void parse_tokens();
private:
	vector<Token> tokens;
};