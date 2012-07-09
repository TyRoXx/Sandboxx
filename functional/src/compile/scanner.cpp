#include "scanner.hpp"


namespace fct
{
	namespace
	{
		bool isSpace(char c)
		{
			return (c >= '\0') && (c <= ' ');
		}

		bool isOperator(char c)
		{
			switch (c)
			{
			case '(':
			case ')':
				return true;

			default:
				return false;
			}
		}
	}


	TokenSequence Scanner::scan(
		const Source &source)
	{
		TokenSequence tokens;

		for (auto i = begin(source), end = std::end(source); i != end; )
		{
			if (isSpace(*i))
			{
				++i;
				continue;
			}
			else if (isOperator(*i))
			{
				tokens.push_back(Token(i, i + 1));
				++i;
			}
			else
			{
				const auto s = i;
				++i;
				for (;
					(i != end) &&
					!isSpace(*i) &&
					!isOperator(*i);
					++i)
				{
				}
				tokens.push_back(Token(s, i));
			}
		}

		return tokens;
	}
}
