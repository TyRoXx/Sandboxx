#include <iostream>
#include <typeinfo>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <fstream>
#include <utility>
#include <vector>

namespace pro
{
	template <class I>
	I begin(std::pair<I, I> const &range)
	{
		return range.first;
	}

	template <class I>
	I end(std::pair<I, I> const &range)
	{
		return range.second;
	}

	template <class ...Elements>
	struct sequence
	{
		template <class Visitor>
		static void accept(Visitor &&visitor)
		{
			visitor.visit(sequence<Elements...>());
		}
	};

	template <class T>
	struct primitive
	{
		template <class Visitor>
		static void accept(Visitor &&visitor)
		{
			visitor.visit(primitive<T>());
		}
	};

	template <class Size, class Element>
	struct vector
	{
		template <class Visitor>
		static void accept(Visitor &&visitor)
		{
			visitor.visit(vector<Size, Element>());
		}
	};

	template <std::uintmax_t N>
	struct fixed_size
	{
		template <class Visitor>
		static void accept(Visitor &&visitor)
		{
			visitor.visit(fixed_size<N>());
		}
	};

	template <class Char>
	struct c_string
	{
		template <class Visitor>
		static void accept(Visitor &&visitor)
		{
			visitor.visit(c_string<Char>());
		}
	};
}

using namespace pro;

struct structure_printer
{
	template <class Head, class ...Tail>
	void visit(sequence<Head, Tail...>)
	{
		Head::accept(*this);
		std::cout << '\n';
		sequence<Tail...>::accept(*this);
	}

	void visit(sequence<>)
	{
	}
	
	template <class T>
	void visit(primitive<T>)
	{
		std::cout << "primitive " << typeid(T).name();
	}
	
	template <class Size, class Element>
	void visit(vector<Size, Element>)
	{
		std::cout << "vector<";
		Size::accept(*this);
		std::cout << ", ";
		Element::accept(*this);
		std::cout << '>';
	}

	template <std::uintmax_t N>
	void visit(fixed_size<N>)
	{
		std::cout << N;
	}

	template <class Char>
	void visit(c_string<Char>)
	{
		std::cout << "C string";
	}
};

template <class Serializer, class Element>
struct poly_lambda
{
	Serializer *serializer;
	
	template <class T>
	void operator ()(T const &element) const
	{
		serializer->serialize(Element(), element);
	}
};

struct binary_serializer
{
	std::ostream &sink;

	explicit binary_serializer(std::ostream &sink)
		: sink(sink)
	{
	}

	template <class T>
	void serialize(primitive<T>, T value)
	{
		sink.write(reinterpret_cast<char const *>(&value), sizeof(value));
	}

	template <class Size, class Element, class Range>
	void serialize(vector<Size, Element>, Range const &range)
	{
		for_each<Element>(Size(), range, poly_lambda<binary_serializer, Element>{this});
	}

	template <class Char, class String>
	void serialize(c_string<Char>, String const &string)
	{
		for (Char const c : string)
		{
			sink.write(reinterpret_cast<char const *>(&c), sizeof(c));
		}
		Char const zero = 0;
		sink.write(reinterpret_cast<char const *>(&zero), sizeof(zero));
	}

	template <class Element, class Range, class Action>
	void for_each(fixed_size<0>, Range const &range, Action const &)
	{
		if (begin(range) != end(range))
		{
			throw std::runtime_error("Too much range input");
		}
	}

	template <class Element, std::uintmax_t N, class Range, class Action>
	void for_each(fixed_size<N>, Range const &range, Action const &action)
	{
		if (begin(range) == end(range))
		{
			throw std::runtime_error("Insufficient range input");
		}

		action(*begin(range));

		for_each<Element>(fixed_size<N - 1>(),
		                  std::make_pair(begin(range) + 1, end(range)),
		                  action);
	}

	template <class HeadDef, class ...TailDef, class Head, class ...Tail>
	void serialize(sequence<HeadDef, TailDef...>, Head &&head, Tail &&...tail)
	{
		serialize(HeadDef(), std::forward<Head>(head));
		serialize(sequence<TailDef...>(), std::forward<Tail>(tail)...);
	}

	template <class Missing, class ...RestDef>
	void serialize(sequence<Missing, RestDef...>)
	{
		static_assert(sizeof(Missing) == -1, "Arguments missing");
	}

	template <class Overflow, class ...Rest>
	void serialize(sequence<>, Overflow const &, Rest const &...)
	{
		static_assert(sizeof(Overflow) == -1, "Too many arguments");
	}

	void serialize(sequence<>)
	{
	}
};

int main()
{
	typedef sequence<
		primitive<int>,
		primitive<double>,
		vector<fixed_size<3>, primitive<float>>,
		c_string<char>,
		primitive<bool>
		> message;

	message::accept(structure_printer());

	std::ofstream bin_file("file.bin", std::ios::binary);
	binary_serializer serializer(bin_file);

	serializer.serialize(message(), 0,
	                                1.2,
	                                std::vector<float>{0.0f, 0.0f, 0.0f},
	                                std::string("hallo"),
	                                true);
}
