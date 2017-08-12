#include <iostream>
#include <exception>
#include "Interpreter.h"
#include "Lexer.h"


Interpreter::Interpreter(Lexer* lexer)
{
	Token token = lexer->getNext();

	while (token.getType() != Token::END_OF_FILE)
	{
		if (token.getType() == Token::UNSET)
		{
			throw std::exception("Token error: lexing didn't set the token");
		}
		else if (token.getType() == Token::COMMENT)
		{
			//just ignore comments
		}
		else if (token.getType() == Token::NEW_LINE)
		{
			//new lines are important to seperate code blocks, but if there are
			//redundant ones, ignore them
		}
		else if (token.getType() == Token::PRINT)
		{
			//for now only numbers can get print
			token = lexer->getNext();

			switch (token.getType())
			{
			case Token::NUMBER:
				std::wcout << token.getNumber() << std::endl;
				break;
			case Token::STRING:
				std::wcout << token.getString() << std::endl;
				break;
			case Token::NEW_LINE:
			case Token::END_OF_FILE:
				//empty print statement - print a newline
				std::wcout << std::endl;
				break;
			default:
				throw std::exception("Can't print anything but numbers or strings");
			}
		}
		else
		{
			//unhandled token
			throw std::exception("Unexpected token");
		}

		token = lexer->getNext();
	}
}