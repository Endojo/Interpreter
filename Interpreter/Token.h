#pragma once

#include <string>

class Token
{
public:
	enum Type
	{
		END_OF_FILE,
		UNSET,
		NEW_LINE,
		PRINT,
		NUMBER,
		STRING,
		NAME,
		COMMENT
	};

	Token(Type t = UNSET, const std::wstring &str = L"")
		: mType(t)
		, mString(str)
	{
	}

	Token(double d)
		: mType(NUMBER)
		, mNumber(d)
	{
	}

	Type getType() { return mType; }
	std::wstring getString() { return mString; }
	double getNumber() { return mNumber; }

private:
	Type mType;
	std::wstring mString;
	double mNumber;
};