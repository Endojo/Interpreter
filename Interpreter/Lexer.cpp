#include "Lexer.h"
#include <fstream>
#include <map>
#include <exception>
#include <locale>
#include <codecvt>
#include <string>


static const std::map<std::wstring, Token::Type> Symbols =
{
	{L"\n", Token::NEW_LINE},
	{L"#", Token::COMMENT},
	{L"\"", Token::STRING}
};

static const std::map<std::wstring, Token::Type> Keywords =
{
	{L"print", Token::PRINT}
};




Lexer::Lexer(const std::wstring &fileName) : mStreamPos(0)
{
	//read in file

	std::string content;
	std::ifstream in(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if (in)
	{
		content.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&content[0], content.size());
		in.close();

		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		mFileContent = converter.from_bytes(content);
	}

	//lex it

	while (!isEOF())
	{
		wchar_t c = mFileContent[mContentPos];

		if (isWS(c))
			skipWS();
		else if (isChar(c))
			parseName();
		else if (isDigit(c))
			parseNumber();
		else
			parseSymbol();
	}
}

Token Lexer::getNext()
{
	if (mStreamPos >= mTokenStream.size())
		return Token(Token::END_OF_FILE);

	Token token = mTokenStream[mStreamPos];
	mStreamPos++;

	return token;
}

Token Lexer::peekNext()
{
	if (mStreamPos >= mTokenStream.size() - 1)
		return Token(Token::END_OF_FILE);

	return mTokenStream[mStreamPos + 1];
}

bool Lexer::isEOF()
{
	return mContentPos == mFileContent.size();
}

bool Lexer::isWS(wchar_t c)
{
	const std::wstring whitespaces = L" \t\v\f\r";

	return whitespaces.find(c) != std::wstring::npos;
}

bool Lexer::isChar(wchar_t c)
{
	const std::wstring chars = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";

	return chars.find(c) != std::wstring::npos;
}

bool Lexer::isDigit(wchar_t c)
{
	const std::wstring digits = L"0123456789";

	return digits.find(c) != std::wstring::npos;
}

void Lexer::skipWS()
{
	while (!isEOF() && isWS(mFileContent[mContentPos]))
		mContentPos++;
}

void Lexer::parseName()
{
	std::wstring name;
	wchar_t c = mFileContent[mContentPos];

	while (isChar(c) || isDigit(c))
	{
		name += c;
		mContentPos++;

		if (isEOF())
			break;

		c = mFileContent[mContentPos];
	}

	//after the name got parsed, we have to check if it is a reserved key word

	for (auto it = Keywords.begin(); it != Keywords.end(); ++it)
	{
		if (name == it->first)
		{
			mTokenStream.push_back(Token(it->second, it->first));
			return;
		}
	}

	//only gets called if the loop didn't return;
	mTokenStream.push_back(Token(Token::NAME, name));
}

void Lexer::parseNumber()
{
	//allow for all C - Style numbers, except they have to start with an digit

	//everything until the next whitespace has to be part of the number

	std::wstring number_str;
	wchar_t c = mFileContent[mContentPos];

	while (!isWS(c))
	{
		number_str += c;
		mContentPos++;

		if (isEOF())
			break;

		c = mFileContent[mContentPos];
	}

	double number = std::stod(number_str);

	mTokenStream.push_back(Token(number));
}

void Lexer::parseString()
{
	std::wstring string;
	wchar_t c = 0;

	//skip leading "
	mContentPos++;

	while (!isEOF())
	{
		c = mFileContent[mContentPos];

		if (c == '\"')
			break;

		if (c == '\n')
			throw std::exception("Didn't find closing \"");

		string += c;
		mContentPos++;
	}

	//skip following "
	mContentPos++;

	mTokenStream.push_back(Token(Token::STRING, string));
}

void Lexer::parseSymbol()
{
	//if there is no further action requiered, the symbol gets written to the token stream
	//in case of a comment or string, they get parsed with those functions

	for (auto it = Symbols.begin(); it != Symbols.end(); ++it)
	{
		if (mFileContent.compare(mContentPos, it->first.size(), it->first) == 0)
		{
			switch (it->second)
			{
			case Token::COMMENT:
				parseComment();
				return;
			case Token::STRING:
				parseString();
				return;
			}

			mTokenStream.push_back(Token(it->second, it->first));
			mContentPos += it->first.size();
			return;
		}
	}

	//no match found
	throw std::exception((
		std::string("Unknown symbol ") +
		std::string(1, mFileContent[mContentPos])
		).c_str());
}

void Lexer::parseComment()
{
	std::wstring comment;
	wchar_t c = mFileContent[mContentPos];

	while (c != '\n' && c != '\r')
	{
		comment += c;
		mContentPos++;

		if (isEOF())
			break;

		c = mFileContent[mContentPos];
	}

	//might be important on windows: after a comment whitespace don't get skipped
	//but the carriage return should not be inside the comment either
	if (c == '\r')
		mContentPos++;

	mTokenStream.push_back(Token(Token::COMMENT, comment));
}