#ifndef NEW_LANG_PARSER_HPP
#define NEW_LANG_PARSER_HPP

#include <silicium/source.hpp>
#include <silicium/sink.hpp>

namespace nl
{
	enum class token_type
	{
		dot,
		comma,
		left_parenthesis,
		right_parenthesis,
		space,
		tab,
		newline,
		identifier,
		integer,
		string,
		return_,
		end_of_file
	};

	struct token
	{
		token_type type;
		std::size_t position;
		std::string content;
	};

	bool is_decimal_digit(char c)
	{
		return (c >= '0') && (c <= '9');
	}

	bool is_identifier_head(char c)
	{
		return
			(c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c == '_') ||
			(c == '-');
	}

	bool is_identifier_middle(char c)
	{
		return is_identifier_head(c) || is_decimal_digit(c);
	}

	boost::optional<token> scan_token(Si::source<char> &input)
	{
		auto const first = Si::get(input);
		if (!first)
		{
			return token{token_type::end_of_file, 0, ""};
		}
		switch (*first)
		{
		case '.': return token{token_type::dot, 0, std::string(1, *first)};
		case ',': return token{token_type::comma, 0, std::string(1, *first)};
		case '(': return token{token_type::left_parenthesis, 0, std::string(1, *first)};
		case ')': return token{token_type::right_parenthesis, 0, std::string(1, *first)};
		case ' ': return token{token_type::space, 0, std::string(1, *first)};
		case '\t': return token{token_type::tab, 0, std::string(1, *first)};
		case '\n': return token{token_type::newline, 0, std::string(1, *first)};
		case '"':
			{
				throw std::logic_error("not implemented");
			}

		default:
			{
				if (is_identifier_head(*first))
				{
					throw std::logic_error("not implemented");
				}
				else if (is_decimal_digit(*first))
				{
					throw std::logic_error("not implemented");
				}
				return boost::none;
			}
		}
	}
}

#endif
