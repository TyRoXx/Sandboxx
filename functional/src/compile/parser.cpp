#include "parser.hpp"
#include <exception>


namespace fct
{
	namespace
	{
		typedef TokenSequence::const_iterator Position;


		bool isToken(const Token &token, const std::string &content)
		{
			return std::string(begin(token), end(token)) == content;
		}

		Tree parseTree(Position &p, Position end)
		{
			if (p == end)
			{
				throw std::runtime_error("Tree expected");
			}

			std::string symbol((begin(*p)), std::end(*p));
			switch (symbol.front())
			{
			case '(':
			case ')':
				throw std::runtime_error("Symbol expected instead of operator");

			default:
				break;
			}

			++p;

			if (p == end ||
				!isToken(*p, "("))
			{
				return Tree(symbol);
			}

			++p;

			std::vector<Tree> arguments;
			for (;;)
			{
				if (p == end)
				{
					throw std::runtime_error("Unexpected end of arguments");
				}

				if (isToken(*p, ")"))
				{
					++p;
					break;
				}

				arguments.push_back(parseTree(p, end));
			}

			return Tree(symbol, arguments);
		}
	}

	Tree Parser::parse(
		const TokenSequence &tokens
		)
	{
		Position p = begin(tokens);
		auto result = parseTree(p, end(tokens));
		if (p != end(tokens))
		{
			throw std::runtime_error("End of source expected after top-level tree");
		}
		return result;
	}
}
