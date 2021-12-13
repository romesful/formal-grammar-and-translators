#pragma once

enum EType
{
	et_intger,
	et_boolean,
	et_real,
	et_char,
	et_string
};

class Type
{
public:
	const EType my_type;
	
	Type() = 0;
	virtual bool can_cast_to(Type another_type) = 0;
};

class IntegerType : Type
{
	IntegerType() { my_type = et_intger; }
	bool can_cast_to(Type another_type)
	{
		return another_type == et_real ||
			another_type == my_type;
	}
};

class BoolType : Type
{
	BoolType() { my_type = et_bool; }
	bool can_cast_to(Type another_type)
	{
		return another_type == et_real ||
			another_type == et_intger  ||
			another_type == et_real    ||
			another_type == my_type;
	}
};

class RealType : Type
{
	RealType() { my_type = et_real; }
	bool can_cast_to(Type another_type)
	{
		return another_type == my_type;
	}
};

class CharType : Type
{
	CharType() { my_type = et_char; }
	bool can_cast_to(Type another_type)
	{
		return another_type == et_string || another_type == my_type;
	}
};

class StringType : Type
{
	StringType() { my_type = et_string; }
	bool can_cast_to(Type another_type)
	{
		return another_type == my_type;
	}
};