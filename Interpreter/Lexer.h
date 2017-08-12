#pragma once

#include <string>
#include <vector>
#include "Token.h"

class Lexer
{
public:
	Lexer(const std::wstring &fileName);

	Token getNext();
	Token peekNext();

private:
	bool isEOF();
	bool isWS(wchar_t c);
	bool isChar(wchar_t c);
	bool isDigit(wchar_t c);
	void skipWS();
	void parseName();
	void parseNumber();
	void parseString();
	void parseSymbol();
	void parseComment();

	std::wstring mFileContent;
	size_t mContentPos;

	std::vector<Token> mTokenStream;
	size_t mStreamPos;
};