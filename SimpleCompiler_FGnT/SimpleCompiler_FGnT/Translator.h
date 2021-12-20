#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <map>
#include <msclr\marshal_cppstd.h>

using namespace std;

using namespace System;
using namespace System::Reflection::Emit;
using namespace System::Reflection;
using System::Reflection::AssemblyName;
using namespace System::Threading;
using namespace msclr::interop;
using namespace System::Collections::Generic;

ref class Translator
{
private:
	int variable_counter = 0;

	ILGenerator^ il_generator;
	Dictionary<String^, LocalBuilder^> variable_lb;
	Dictionary<String^, System::Type^> variable_type;

	vector<Token*>* tokens = new vector<Token*>();
	int current_token_position;
	Token* current_token;
	OperatorType previous_operator;

	bool accept(TType token_type);
	bool accept(OperatorType operator_type);
	bool accept(vector<OperatorType> operator_types);

	void next_token();

	System::Type^ get_type_from_token(Token* token);
	String^ get_var_name_from_token(Token* token);
	void add_var(String^ name, System::Type^ type);
	void emit_by_operation(OperatorType op);

	String^ convert_to_managed_string(string str);

	void change_sign(System::Type^ type);
	void load_const(Token* token);
	void call_writeln(System::Type^ type);
	void call_readln(Token* token);

	void program();

	void block();

	void vars_section();
	void operators_section();

	bool single_var_definition();

	System::Type^ type();

	void operator_();
	bool simple_operator();
	System::Type^ expression();
	bool relation_operation();
	System::Type^ simple_expression();
	bool additive_operation();
	System::Type^ term();
	bool multiplicative_operation();
	System::Type^ factor();
	bool sign();
	void complex_operator();
	void neccessary_compound_operator();
	bool compound_operator();
	bool if_operator();
	bool while_operator();
	bool writeln();
	bool readln();
public:
	Translator() {};
	Translator(vector<Token*> _tokens);

	~Translator();
};