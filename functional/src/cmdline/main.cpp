#include "compile/parser.hpp"
#include "run/interpreter.hpp"
#include "run/primitives.hpp"
#include <iostream>
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
}

int main()
{
	std::string line;
	while (getline(cin, line) &&
		!line.empty())
	{
		try
		{
			const auto program = Parser::parse(
				Scanner::scan(line));

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
