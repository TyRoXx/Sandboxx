#include "function.hpp"
#include <iostream>

int main()
{
	exp::function<
		bool (std::string const &),
		exp::default_value_on_empty_call,
		exp::ptr_to_polymorphic_storage> f;

	assert(!f);
	f("");
	assert(f == f);
	assert(!(f < f));
	f = f;
	assert(!f);
	f = std::move(f);
	assert(!f);
	auto g = f;
	assert(!g);
	assert(!f);
	auto h = std::move(g);
	assert(!h);
	g = std::move(h);
	assert(!g);
	h = g;
	assert(!h);
	swap(h, g);
	h.swap(g);
	f = [](std::string const &s) -> bool
	{
			return std::cout << s << '\n';
	};
	assert(f);
	assert(f == f);
	decltype(h) i = [](std::string const &) -> bool { return {}; };
	f("Hello, world!");
}
