#include "function.hpp"
#include <iostream>
#include <cassert>

int main()
{
	exp::function<bool (std::string const &), exp::default_value_on_empty_call> f;
	f("");
	f = [](std::string const &s) -> bool
	{
			return std::cout << s << '\n';
	};
	f("Hello, world!");
}
