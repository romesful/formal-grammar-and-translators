#pragma once

enum EType
{
	et_intger,
	et_bool,
	et_real,
	et_char,
	et_string,
	et_undefined
};

class Type
{
public:
	EType type = et_undefined;
	Type();
	virtual bool can_cast_to(Type another_type);
};

class IntegerType : Type
{
public:
	IntegerType() { type = et_intger; }
	bool can_cast_to(Type another_type)
	{
		return another_type.type == et_real ||
			another_type.type == type;
	}
};

class BoolType : Type
{
public:
	BoolType() { type = et_bool; }
	bool can_cast_to(Type another_type)
	{
		return another_type.type == et_real ||
			another_type.type == et_intger  ||
			another_type.type == et_real    ||
			another_type.type == type;
	}
};

class RealType : Type
{
public:
	RealType() { type = et_real; }
	bool can_cast_to(Type another_type)
	{
		return another_type.type == type;
	}
};

class CharType : Type
{
public:
	CharType() { type = et_char; }
	bool can_cast_to(Type another_type)
	{
		return another_type.type == et_string || another_type.type == type;
	}
};

class StringType : Type
{
public:
	StringType() { type = et_string; }
	bool can_cast_to(Type another_type)
	{
		return another_type.type == type;
	}
};