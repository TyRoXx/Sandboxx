#include "tree.hpp"


namespace fct
{
	Tree::Tree()
	{
	}

	Tree::Tree(Tree &&other)
	{
		swap(other);
	}

	Tree::Tree(const Tree &other)
		: symbol(other.symbol)
		, arguments(other.arguments)
	{
	}

	Tree::Tree(std::string symbol)
		: symbol(std::move(symbol))
	{
	}

	Tree::Tree(std::string symbol, Arguments arguments)
		: symbol(std::move(symbol))
		, arguments(std::move(arguments))
	{
	}

	Tree &Tree::operator = (Tree &&other)
	{
		swap(other);
		return *this;
	}

	Tree &Tree::operator = (const Tree &other)
	{
		symbol = other.symbol;
		arguments = other.arguments;
		return *this;
	}

	void Tree::swap(Tree &other)
	{
		std::swap(symbol, other.symbol);
		std::swap(arguments, other.arguments);
	}


	void swap(Tree &left, Tree &right)
	{
		left.swap(right);
	}

	bool operator == (const Tree &left, const Tree &right)
	{
		return (left.symbol == right.symbol) &&
			(left.arguments == right.arguments);
	}

	bool operator != (const Tree &left, const Tree &right)
	{
		return !(left == right);
	}


	std::ostream &operator << (std::ostream &os, const Tree &tree)
	{
		os
			<< tree.symbol;

		const auto &args = tree.arguments;
		if (!args.empty())
		{
			os << '(';
			for (Tree::Arguments::const_iterator i = begin(args), e = end(args) - 1; i != e; ++i)
			{
				os << *i << ' ';
			}
			os << args.back();
			os << ')';
		}

		return os;
	}
}
