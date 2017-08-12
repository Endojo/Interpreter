#include <string>
#include <iostream>
#include <exception>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <locale>
#include <codecvt>

#include "Lexer.h"
#include "Interpreter.h"

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
	_setmode(_fileno(stdout), _O_U16TEXT);

	std::wstring sourceFile = L"H:\\Jonathan\\repros\\Interpreter\\Scripts\\Test.src";

	if (argc > 1)
		sourceFile = argv[1];

	size_t first = sourceFile.find_last_of(L'\\') + 1;
	size_t last = sourceFile.find_last_of(L'.');
	std::wstring wname = sourceFile.substr(first, last - first);
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string name = converter.to_bytes(wname);

	SetConsoleTitle((char*)name.c_str());

	try
	{
		Lexer lexer(sourceFile);

		Interpreter interpreter(&lexer);
	}
	catch (const std::exception& e)
	{
		std::wcout << e.what() << std::endl;
	}

	std::cin.get();
}