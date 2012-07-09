#include "compile/parser.hpp"
#include "run/interpreter.hpp"
#include "run/primitives.hpp"
#include <iostream>
#include <fstream>
using namespace fct;
using namespace std;


namespace
{
	void addPrimitives(Interpreter &interpreter)
	{
		interpreter.pushSymbol("define", std::unique_ptr<Object>(new Define));
		interpreter.pushSymbol("if", std::unique_ptr<Object>(new If));
		interpreter.pushSymbol("not", std::unique_ptr<Object>(new Not));
		interpreter.pushSymbol("lambda", std::unique_ptr<Object>(new MakeLambda));
		interpreter.pushSymbol("less", std::unique_ptr<Object>(new LessThan));
		interpreter.pushSymbol("add", std::unique_ptr<Object>(new Add));
		interpreter.pushSymbol("sub", std::unique_ptr<Object>(new Subtract));
	}

	void run(const std::string &code)
	{
		try
		{
			const auto program = Parser::parse(
				Scanner::scan(code));

			Interpreter interpreter;
			addPrimitives(interpreter);
			const auto result = interpreter.evaluate(program);
			if (result)
			{
				cout << *result << endl;
			}
			else
			{
				cerr << "No result" << endl;
			}
		}
		catch (const std::runtime_error &e)
		{
			cerr << e.what() << endl;
		}
	}
}

int main(int argc, const char **argv)
{
	if (argc >= 2)
	{
		for (size_t i = 1; i < static_cast<size_t>(argc); ++i)
		{
			const std::string fileName = argv[i];
			std::ifstream file(fileName, std::ios::binary);
			if (!file)
			{
				cerr << "Cannot open file " << fileName << endl;
				return 1;
			}

			const std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			run(code);
		}
		return 0;
	}

	std::string line;
	while (getline(cin, line) &&
		!line.empty())
	{
		run(line);
	}
}
