#include "pch.h"
#include "Translator.h"

Translator::Translator(vector<Token*> _tokens)
{
	tokens = &_tokens;

	current_token_position = 0;
	previous_operator = otError;
	next_token();

	String^ assemblyName = "MyProgram";
	String^ modName = "MyProgram.exe";
	String^ typeName = "MyProgram";
	String^ methodName = "Main";


	//Уникальный идентификатор сборки
	AssemblyName^ name = gcnew AssemblyName(assemblyName);

	//Домен приложения
	AppDomain^ domain = Threading::Thread::GetDomain();

	//Определяет и представляет динамическую сборку
	AssemblyBuilder^ builder = domain->DefineDynamicAssembly(name, AssemblyBuilderAccess::RunAndSave);

	//Определяет и представляет модуль в динамической сборке
	ModuleBuilder^ module = builder->DefineDynamicModule(modName, true);

	//Определяет и создает новые экземпляры классов во время выполнения
	TypeBuilder^ typeBuilder = module->DefineType(typeName, TypeAttributes::Public);

	//Определяет и представляет метод для динамического класса
	MethodBuilder^ methodBuilder = typeBuilder->DefineMethod(methodName, MethodAttributes::Static
		| MethodAttributes::Public, void::typeid, gcnew cli::array<Type^>{});


	il_generator = methodBuilder->GetILGenerator();

	// generate

	/*
	il_generator->DeclareLocal(int::typeid);
	il_generator->Emit(OpCodes::Ldc_I4, 10);
	il_generator->Emit(OpCodes::Stloc, 0);

	auto x = il_generator->DefineLabel();
	auto y = il_generator->DefineLabel();
	
	il_generator->MarkLabel(x);
	il_generator->Emit(OpCodes::Ldloc, 0);
	il_generator->Emit(OpCodes::Ldc_I4, 100);
	il_generator->Emit(OpCodes::Clt);
	il_generator->Emit(OpCodes::Brfalse, y);

	il_generator->Emit(OpCodes::Ldloc, 0);
	il_generator->Emit(OpCodes::Ldc_I4, 10);
	il_generator->Emit(OpCodes::Add);
	il_generator->Emit(OpCodes::Stloc, 0);
	il_generator->Emit(OpCodes::Ldloc, 0);
	auto writeln_test = Console::typeid->GetMethod("WriteLine", gcnew cli::array<Type^>{int::typeid});
	il_generator->Emit(OpCodes::Call, writeln_test);
	il_generator->Emit(OpCodes::Br, x);
	il_generator->MarkLabel(y);

	//typeof(Console).GetMethod("WriteLine", new Type[] { typeof(string) })
	auto writeln_test = Console::typeid->GetMethod("ReadLine", gcnew cli::array<Type^>{});
	il_generator->Emit(OpCodes::Call, writeln_test);
	auto lb = il_generator->DeclareLocal(int::typeid);
	il_generator->Emit(OpCodes::Stloc, lb);
	*/
	
	program();
	// end generate

	// чтобы консоль не сразу закрывалась
	il_generator->EmitWriteLine(convert_to_managed_string("Press any button..."));
	auto readln_method = Console::typeid->GetMethod("ReadLine", gcnew cli::array<Type^>{});
	il_generator->Emit(OpCodes::Call, readln_method);
	auto lb = il_generator->DeclareLocal(String::typeid);
	il_generator->Emit(OpCodes::Stloc, lb);
	// 

	il_generator->Emit(OpCodes::Ret);

	builder->SetEntryPoint(methodBuilder);

	Type^ myClass = typeBuilder->CreateType();

	//Сохраняем в *.exe
	builder->Save(modName);
}

Translator::~Translator()
{
	delete il_generator;
}

bool Translator::accept(TType token_type)
{
	bool result = true;
	if (current_token->token_type != token_type)
		result = false;

	if (result)
		next_token();

	return result;
}

bool Translator::accept(OperatorType operator_type)
{
	bool result = true;
	if (current_token->token_type != ttOperator)
		result = false;

	if (((OperatorToken*)current_token)->operator_type != operator_type)
		result = false;

	if (result)
		next_token();

	return result;
}

bool Translator::accept(vector<OperatorType> operator_types)
{
	bool result = false;
	if (current_token->token_type == ttOperator)
	{
		OperatorType current_type = ((OperatorToken*)current_token)->operator_type;

		for (OperatorType operator_type : operator_types)
		{
			if (operator_type == current_type)
			{
				result = true;
				break;
			}
		}
	}

	if (result)
		next_token();

	return result;
}

void Translator::next_token()
{
	if (current_token_position == tokens->size())
		return;

	if (current_token != NULL && current_token->token_type == ttOperator)
	{
		previous_operator = ((OperatorToken*)current_token)->operator_type;
	}

	current_token = (*tokens)[current_token_position];
	current_token_position++;
}

String^ Translator::get_var_name_from_token(Token* token)
{
	string result = "";
	if (token->token_type == ttIdentificator)
		result = ((IdentificatorToken*)token)->name;

	return convert_to_managed_string(result);
}

Type^ Translator::get_type_from_token(Token* token)
{
	if (auto ct = dynamic_cast<ConstToken<int>*>(token)) {
		return int::typeid;
	}
	else if (auto ct = dynamic_cast<ConstToken<double>*>(token)) {
		return double::typeid;
	}
	else if (auto ct = dynamic_cast<ConstToken<string>*>(token)) {
		return string::typeid;
	}
	else if (auto ct = dynamic_cast<ConstToken<char>*>(token)) {
		return char::typeid;
	}
	else
		return bool::typeid;
}

void Translator::add_var(String^ name, Type^ type)
{
	variable_lb[name]	= il_generator->DeclareLocal(type);
	variable_type[name] = type;
}

void Translator::emit_by_operation(OperatorType op)
{
	/*
	{"+", otPlus},
	{"-", otMinus},
	{"/", otSlash},
	{"*", otStar},
	{"=", otEqual},
	{"<", otLess},
	{">", otGreater},
	{">=", otGreaterEqual},
	{"<=", otLessEqual},
	{"<>", otLessGreater}*/
	/*
	otDiv,					// div
	otAnd,					// and
	otNot,					// not
	otMod,					// mod
	otOr
	*/
	switch (op)
	{
		case otPlus:			il_generator->Emit(OpCodes::Add); break;
		case otMinus:			il_generator->Emit(OpCodes::Sub); break;
		case otSlash:			il_generator->Emit(OpCodes::Div); break;
		case otStar:			il_generator->Emit(OpCodes::Mul); break;
		case otDiv:				il_generator->Emit(OpCodes::Div); break;
		case otMod:				il_generator->Emit(OpCodes::Rem); break;

		case otAnd:				il_generator->Emit(OpCodes::And); break;
		case otOr:				il_generator->Emit(OpCodes::Or);  break;
		case otNot:				il_generator->Emit(OpCodes::Not); break;

		case otEqual:			il_generator->Emit(OpCodes::Ceq); break;
		case otLess:			il_generator->Emit(OpCodes::Clt); break;
		case otGreater:			il_generator->Emit(OpCodes::Cgt); break;

		case otGreaterEqual:
			il_generator->Emit(OpCodes::Clt);
			il_generator->Emit(OpCodes::Ldc_I4_0);
			il_generator->Emit(OpCodes::Ceq);
			break;
		case otLessEqual:
			il_generator->Emit(OpCodes::Cgt);
			il_generator->Emit(OpCodes::Ldc_I4_0);
			il_generator->Emit(OpCodes::Ceq);
			break;
		case otLessGreater:
			il_generator->Emit(OpCodes::Ceq);
			il_generator->Emit(OpCodes::Ldc_I4_0);
			il_generator->Emit(OpCodes::Ceq);
			break;

	}
}

void Translator::program() // <программа>::=program <имя>(<имя файла>{,<имя файла>});<блок>.
{
	accept(otProgram);
	accept(ttIdentificator);
	accept(otSemiColon);

	block();

	accept(otDot);
}

String^ Translator::convert_to_managed_string(string str)
{
	return marshal_as<String^>(str);
}

void Translator::block() // <блок>::=<раздел констант><раздел типов><раздел переменных><раздел процедур и функций><раздел операторов>
{
	vars_section();
	operators_section();
}

// ======== Раздел переменных ========
bool Translator::single_var_definition() // <описание однотипных переменных>::=<имя>{,<имя>}:<тип>
{
	List<String^> variableNames;

	variableNames.Add(get_var_name_from_token(current_token));
	if (!accept(ttIdentificator))
		return false;

	while (accept(otComma))
	{
		variableNames.Add(get_var_name_from_token(current_token));
		accept(ttIdentificator);
	}

	accept(otColon);

	Type^ varType = type();

	for (int i = 0; i < variableNames.Count; i++)
	{
		String^ name = variableNames[i];
		add_var(name, varType);
	}

	return true;
}

void Translator::call_writeln(System::Type^ type)
{
	auto writeln_method = Console::typeid->GetMethod("WriteLine", gcnew cli::array<Type^>{type});
	il_generator->Emit(OpCodes::Call, writeln_method);
}

void Translator::call_readln(Token* token)
{
	String^ name		= get_var_name_from_token(token);
	Type^ type			= variable_type[name];
	LocalBuilder^ lb	= variable_lb[name];

	MethodInfo^ readln_method = Console::typeid->GetMethod("ReadLine", gcnew cli::array<Type^>{});
	il_generator->Emit(OpCodes::Call, readln_method);

	MethodInfo^ convert_method = nullptr;

	if (type == int::typeid)
	{
		convert_method = Convert::typeid->GetMethod("ToInt32", gcnew cli::array<Type^>{string::typeid});
	}
	else if (type == double::typeid)
	{
		convert_method = Convert::typeid->GetMethod("ToDouble", gcnew cli::array<Type^>{string::typeid});
	}
	else if (type == bool::typeid)
	{
		convert_method = Convert::typeid->GetMethod("ToBoolean", gcnew cli::array<Type^>{string::typeid});
	}
	else if (type == char::typeid)
	{
		convert_method = Convert::typeid->GetMethod("ToChar", gcnew cli::array<Type^>{string::typeid});
	}
	
	if (convert_method != nullptr)
		il_generator->Emit(OpCodes::Call, convert_method);
	il_generator->Emit(OpCodes::Stloc, lb);
}

Type^ Translator::type() // <тип>::=integer|real|string|char
{
	if (accept(otInteger))
		return int::typeid;
	if (accept(otReal))
		return double::typeid;
	if (accept(otString))
		return String::typeid;
	if (accept(otChar))
		return Char::typeid;
}

void Translator::vars_section() // <раздел переменных>::= var <описание однотипных переменных>;{<описание однотипных переменных>;} | <пусто>
{
	accept(otVar);

	single_var_definition();
	accept(otSemiColon);

	while (single_var_definition())
	{
		accept(otSemiColon);
	}
}

// ======== Раздел операторов ========
// <раздел операторов>::= <составной оператор>

void Translator::operators_section()
{
	neccessary_compound_operator();
}

void Translator::change_sign(System::Type^ type)
{
	if (type == int::typeid) {
		il_generator->Emit(OpCodes::Ldc_I4, -1);
		il_generator->Emit(OpCodes::Mul);
	}
	else if (type == double::typeid) {
		il_generator->Emit(OpCodes::Ldc_R8, -1);
		il_generator->Emit(OpCodes::Mul);
	}
}

void Translator::load_const(Token* token)
{
	if (auto ct = dynamic_cast<ConstToken<int>*>(token)) {
		il_generator->Emit(OpCodes::Ldc_I4, ct->value);
	}
	else if (auto ct = dynamic_cast<ConstToken<double>*>(token)) {
		il_generator->Emit(OpCodes::Ldc_R8, ct->value);
	}
	else if (auto ct = dynamic_cast<ConstToken<string>*>(token)) {
		il_generator->Emit(OpCodes::Ldstr, convert_to_managed_string(ct->value));
	}
	else if (auto ct = dynamic_cast<ConstToken<char>*>(token)) {
		il_generator->Emit(OpCodes::Ldstr, convert_to_managed_string(string(1, ct->value)));
	}
	else if (auto ct = dynamic_cast<ConstToken<bool>*>(token)) {
		il_generator->Emit(OpCodes::Ldc_I4, ct->value);
	}
}

//<оператор>::=<простой оператор>|<сложный оператор>
void Translator::operator_()
{
	if (!simple_operator())
		complex_operator();
}

//<простой оператор>::=<переменная>:=<выражение>
bool Translator::simple_operator() // *
{
	int mem_position = current_token->position;
	String^ name = get_var_name_from_token(current_token);

	if (!accept(ttIdentificator))
		return false;

	accept(otAssign);
	Type^ t = expression();

	// кладем значение
	il_generator->Emit(OpCodes::Stloc, variable_lb[name]);

	return true;
}

//<выражение>::=<простое выражение>|<простое выражение><операция отношения><простое выражение>
Type^ Translator::expression()
{
	Type^ t1, ^ t2;
	t1 = simple_expression();
	if (relation_operation())
	{
		OperatorType mem_op = previous_operator;
		t2 = simple_expression();
		emit_by_operation(mem_op);
	}

	return t1;
}

//<операция отношения>::= =|<>|<|<=|>=|>
bool Translator::relation_operation()  // *
{
	return accept({ otEqual, otLessGreater, otLess, otLessEqual, otGreaterEqual, otGreater });
}

//<простое выражение>::=<слагаемое>{<аддитивная операция><слагаемое>}
Type^ Translator::simple_expression()
{
	Type ^t1, ^t2;
	t1 = term();
	while (additive_operation())
	{
		OperatorType mem_op = previous_operator;
		t2 = term();
		emit_by_operation(mem_op);
	}
	return t1;
}

//<аддитивная операция>::= +|-|or
bool Translator::additive_operation()  // *
{
	return accept({ otPlus, otMinus, otOr });
}

//<слагаемое>::=<множитель>{<мультипликативная операция><множитель>}
Type^ Translator::term()
{
	Type^ t1, ^t2;
	t1 = factor();
	while (multiplicative_operation())
	{
		OperatorType mem_op = previous_operator;
		t2 = factor();
		emit_by_operation(mem_op);
	}

	return t1;
}

//<мультипликативная операция>::=*|/|div|mod|and
bool Translator::multiplicative_operation()  // *
{
	return accept({ otStar, otSlash, otDiv, otMod, otAnd });
}

//<множитель>::=[<знак>]<переменная>|[<знак>]<константа>|[<знак>](<выражение>)|not <множитель>
Type^ Translator::factor()
{
	// TODO: сделать что-то со знаком...
	OperatorType mem_op = otPlus;
	if (sign())
	{
		mem_op = previous_operator;
	}

	Token* mem_token = current_token;

	if (accept(ttIdentificator))
	{
		String^ name = get_var_name_from_token(mem_token);

		il_generator->Emit(OpCodes::Ldloc, variable_lb[name]);
		auto type = variable_type[name];

		if (mem_op == otMinus)
			change_sign(type);

		return type;
	}
	else if (accept(ttConst))
	{
		Type^ const_type = get_type_from_token(mem_token);

		load_const(mem_token);

		if (mem_op == otMinus)
			change_sign(const_type);

		return const_type;
	}
	else if (accept(otLeftParenthesis))
	{
		Type^ type = expression();
		accept(otRightParenthesis);

		if (mem_op == otMinus)
			change_sign(type);

		return type;
	}
	else if (accept(otNot))
	{
		Type^ t = factor();

		emit_by_operation(otNot);

		return bool::typeid;
	}

}

//<знак>::= +|-
bool Translator::sign()  // *
{
	return accept({ otPlus, otMinus });
}

//<сложный оператор>::=<составной оператор>|<выбирающий оператор>|<оператор цикла>|<врайтлайн>
void Translator::complex_operator()
{
	if (compound_operator())
	{
		//...
	}
	else if (if_operator())
	{
		//...
	}
	else if (while_operator())
	{
		//...
	}
	else if (writeln())
	{
		//...
	}
	else if (readln())
	{
		//...
	}
}

//<составной оператор>::= begin <оператор>{;<оператор>} end
bool Translator::compound_operator()  // *
{
	if (!accept(otBegin))
		return false;

	operator_();

	while (accept(otSemiColon))
	{
		operator_();
	}

	accept(otEnd);

	return true;
}

//<обязательный составной оператор>::= begin <оператор>{;<оператор>} end
void Translator::neccessary_compound_operator()  // *
{
	accept(otBegin);

	operator_();

	while (accept(otSemiColon))
	{
		operator_();
	}

	accept(otEnd);
}

//<выбирающий оператор>::= if <выражение> then <оператор>|if <выражение> then <оператор> else <оператор>
bool Translator::if_operator()  // *
{
	/*
	il_generator->DeclareLocal(int::typeid);
	il_generator->Emit(OpCodes::Ldc_I4, 10);
	il_generator->Emit(OpCodes::Stloc, 0);

	auto x = il_generator->DefineLabel();
	auto y = il_generator->DefineLabel();

	il_generator->MarkLabel(x);
	il_generator->Emit(OpCodes::Ldloc, 0);
	il_generator->Emit(OpCodes::Ldc_I4, 100);
	il_generator->Emit(OpCodes::Clt);
	il_generator->Emit(OpCodes::Brfalse, y);

	il_generator->Emit(OpCodes::Ldloc, 0);
	il_generator->Emit(OpCodes::Ldc_I4, 10);
	il_generator->Emit(OpCodes::Add);
	il_generator->Emit(OpCodes::Stloc, 0);
	il_generator->Emit(OpCodes::Ldloc, 0);
	auto writeln_test = Console::typeid->GetMethod("WriteLine", gcnew cli::array<Type^>{int::typeid});
	il_generator->Emit(OpCodes::Call, writeln_test);
	il_generator->Emit(OpCodes::Br, x);
	il_generator->MarkLabel(y);*/
	if (!accept(otIf))
		return false;

	auto then_pointer	= il_generator->DefineLabel();
	auto after_pointer	= il_generator->DefineLabel();

	Type^ t = expression();
	il_generator->Emit(OpCodes::Brfalse, then_pointer);

	accept(otThen);
	operator_();
	il_generator->Emit(OpCodes::Br, after_pointer);

	il_generator->MarkLabel(then_pointer);
	if (accept(otElse))
	{
		operator_();
	}
	il_generator->MarkLabel(after_pointer);

	return true;
}

//<оператор цикла>::= while <выражение> do <оператор>
bool Translator::while_operator()  // *
{

	if (!accept(otWhile))
		return false;

	auto condition_pointer	= il_generator->DefineLabel();
	auto after_pointer		= il_generator->DefineLabel();

	il_generator->MarkLabel(condition_pointer);
	Type^ t = expression();
	il_generator->Emit(OpCodes::Brfalse, after_pointer);

	accept(otDo);

	operator_();
	il_generator->Emit(OpCodes::Br, condition_pointer);
	il_generator->MarkLabel(after_pointer);

	return true;
}

bool Translator::writeln()
{
	if (!accept(otWriteLn))
		return false;

	accept(otLeftParenthesis);
	Type^ t = expression();
	accept(otRightParenthesis);

	call_writeln(t);

	return true;
}

bool Translator::readln()
{
	if (!accept(otReadLn))
		return false;

	accept(otLeftParenthesis);
	Token* mem_token = current_token;
	accept(ttIdentificator);

	call_readln(mem_token);

	accept(otRightParenthesis);

	return true;
}