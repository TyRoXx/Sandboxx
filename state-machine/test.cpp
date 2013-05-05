#include "state_machine.hpp"
#include <iostream>

struct None
{

};

template <std::size_t I>
struct A
{
	A()
	{
		std::cout << I << "\n";
	}

	~A()
	{
		std::cout << I << " dtor\n";
	}
};

int main()
{
	sm::state_machine<None, A<0>, A<1>> m((sm::state<None>()));
	m.enter<A<0>>();
	m.enter<A<1>>();
	m.enter<A<0>>();
}
