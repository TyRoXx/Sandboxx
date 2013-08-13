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

#define STR2(x) #x
#define STR(x) STR2(x)

int main()
{
	{
		sm::state_machine<None, A<0>, A<1>> m((sm::state<None>()));
		m.enter<A<0>>();
		m.enter<A<1>>();
		m.enter<A<0>>();
	}

#define PRINT_SIZEOF(what) std::cout << STR(what) << ' ' << sizeof what << '\n'

	PRINT_SIZEOF((sm::state_machine<int>::state_storage));
	PRINT_SIZEOF((sm::state_machine<int>::storages));
	PRINT_SIZEOF((sm::state_machine<int>::destructor));

	PRINT_SIZEOF((sm::largest_sizeof<None>));
	PRINT_SIZEOF((sm::largest_sizeof<None, A<0>>));
	PRINT_SIZEOF((sm::largest_sizeof<None, size_t>));

	PRINT_SIZEOF((sm::largest_alignof<None>));
	PRINT_SIZEOF((sm::largest_alignof<None, A<0>>));
	PRINT_SIZEOF((sm::largest_alignof<None, size_t>));

	PRINT_SIZEOF((sm::state_machine<None>));
	PRINT_SIZEOF((sm::state_machine<None, A<0>>));
	PRINT_SIZEOF((sm::state_machine<None, A<0>, A<1>>));
}
