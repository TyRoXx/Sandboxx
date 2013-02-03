#include "function.hpp"
#include <iostream>

int main()
{
	exp::function<
		bool (std::string const &),
		exp::default_value_on_empty_call,
		exp::ptr_to_polymorphic_storage<bool (std::string const &)>> f;

	assert(!f);
	f("");
	assert(f == f);
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

	exp::function<int (int), exp::throw_on_empty_call, exp::small_functor_storage<32, int (int)>> j;
	assert(!j);
	assert(j == j);
	j = [](int a) { return a + 1; };
	assert(j(2) == 3);
	assert(j);
	assert(j == j);

	exp::function<int(int), exp::throw_on_empty_call, exp::small_functor_storage<0, int (int)>> k;
	k = [j](int a) { return j(a); };
	assert(j(2) == 3);

	std::cout << "sizeof(f) == " << sizeof(f) << '\n';
	std::cout << "sizeof(j) == " << sizeof(j) << '\n';
	std::cout << "sizeof(k) == " << sizeof(k) << '\n';
}
