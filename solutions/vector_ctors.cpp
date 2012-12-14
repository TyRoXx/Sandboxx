#include <iterator>
#include <iostream>

namespace
{
	void vector_ctor(size_t n, int element)
	{
		std::cout << "constructing from size\n";
	}
	
	//Pseudo-Iterator-Kategorie, um beim "Tag Dispatching" Integer von Iteratoren unterscheiden zu können
	struct int_category
	{
	};
	
	template <class I>
	void real_vector_ctor(I begin, I end, int_category)
	{
		std::cout << "nope, integers\n";
		
		const size_t n = begin;
		const int element = end;
		vector_ctor(n, element);
	}
	
	template <class I>
	void real_vector_ctor(I begin, I end, std::random_access_iterator_tag) //für andere Iteratortypen natürlich äquivalent
	{
		std::cout << "iterators in deed!\n";
	}
	
	template <class I>
	struct iterator_category
	{
		typedef typename std::iterator_traits<I>::iterator_category type;
	};
	
	template <>
	struct iterator_category<int> //für andere Integer-Typen würde man das auch machen
	{
		typedef int_category type; 
	};
	
	template <class I>
	void vector_ctor(I begin, I end)
	{
		std::cout << "here be iterators..\n";

		//ob I tatsächlich ein Iterator ist, wird hier festgestellt
		real_vector_ctor(begin, end, typename iterator_category<I>::type());
	}
}

int main()
{
	vector_ctor(static_cast<size_t>(1), 1);
	std::cout << '\n';
	
	vector_ctor(1, 1);
	std::cout << '\n';
	
	int d;
	vector_ctor(&d, &d);
	std::cout << '\n';
}

//Erwartete Ausgabe:
/*
constructing from size

here be iterators..
nope, integers
constructing from size

here be iterators..
iterators in deed!
*/
