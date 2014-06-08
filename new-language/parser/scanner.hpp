#ifndef NEW_LANG_SCANNER_HPP
#define NEW_LANG_SCANNER_HPP

#include <silicium/source.hpp>
#include <silicium/sink.hpp>
#include <unordered_map>

namespace nl
{
	enum class token_type
	{
		dot,
		comma,
		left_parenthesis,
		right_parenthesis,
		assignment,
		space,
		tab,
		newline,
		identifier,
		integer,
		string,
		return_,
		end_of_file
	};

	char const *get_token_name(token_type token)
	{
		switch (token)
		{
#define NEW_LANG_RETURN_TOKEN_NAME(tk_) case token_type::tk_: return BOOST_STRINGIZE(tk_)
		NEW_LANG_RETURN_TOKEN_NAME(dot);
		NEW_LANG_RETURN_TOKEN_NAME(comma);
		NEW_LANG_RETURN_TOKEN_NAME(left_parenthesis);
		NEW_LANG_RETURN_TOKEN_NAME(right_parenthesis);
		NEW_LANG_RETURN_TOKEN_NAME(assignment);
		NEW_LANG_RETURN_TOKEN_NAME(space);
		NEW_LANG_RETURN_TOKEN_NAME(tab);
		NEW_LANG_RETURN_TOKEN_NAME(newline);
		NEW_LANG_RETURN_TOKEN_NAME(identifier);
		NEW_LANG_RETURN_TOKEN_NAME(integer);
		NEW_LANG_RETURN_TOKEN_NAME(string);
		NEW_LANG_RETURN_TOKEN_NAME(return_);
		NEW_LANG_RETURN_TOKEN_NAME(end_of_file);
#undef NEW_LANG_RETURN_TOKEN_NAME
		}
		return nullptr;
	}

	struct token
	{
		token_type type;
		std::string content;
	};

	inline bool is_decimal_digit(char c)
	{
		return (c >= '0') && (c <= '9');
	}

	inline bool is_identifier_head(char c)
	{
		return
			(c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c == '_') ||
			(c == '-');
	}

	inline bool is_identifier_middle(char c)
	{
		return is_identifier_head(c) || is_decimal_digit(c);
	}

	template <class Element>
	boost::optional<Element> peek(Si::source<Element> &source)
	{
		auto const next = source.map_next(1);
		if (next.empty())
		{
			return boost::none;
		}
		return next.front();
	}

	inline boost::optional<token> scan_token(Si::source<char> &input)
	{
		auto const first = Si::get(input);
		if (!first)
		{
			return token{token_type::end_of_file, ""};
		}
		static std::unordered_map<char, nl::token_type> const tokens =
		{
			{'.', nl::token_type::dot},
			{',', nl::token_type::comma},
			{'(', nl::token_type::left_parenthesis},
			{')', nl::token_type::right_parenthesis},
			{'=', nl::token_type::assignment},
			{' ', nl::token_type::space},
			{'\t', nl::token_type::tab},
			{'\n', nl::token_type::newline}
		};
		if (*first == '"')
		{
			std::string content;
			for (;;)
			{
				auto const next = Si::get(input);
				if (*next == '"')
				{
					return token{token_type::string, std::move(content)};
				}
				switch (*next)
				{
				case '\\':
					{
						auto const escaped = Si::get(input);
						if (!escaped)
						{
							return boost::none;
						}
						switch (*escaped)
						{
						case '\\':
						case '"':
							content.push_back(*escaped);
							break;

						default:
							return boost::none;
						}
						break;
					}

				default:
					content.push_back(*next);
					break;
				}
			}
		}
		{
			auto const single_char_found = tokens.find(*first);
			if (single_char_found != end(tokens))
			{
				return token{single_char_found->second, std::string(1, *first)};
			}
		}
		if (is_identifier_head(*first))
		{
			std::string content(1, *first);
			for (;;)
			{
				auto const next = peek(input);
				if (!next)
				{
					break;
				}
				if (!is_identifier_middle(*next))
				{
					break;
				}
				content.push_back(*next);
				Si::get(input);
			}
			if (content == "return")
			{
				return token{token_type::return_, std::move(content)};
			}
			return token{token_type::identifier, std::move(content)};
		}
		else if (is_decimal_digit(*first))
		{
			std::string content(1, *first);
			for (;;)
			{
				auto const next = peek(input);
				if (!next)
				{
					break;
				}
				if (!is_decimal_digit(*next))
				{
					break;
				}
				content.push_back(*next);
				Si::get(input);
			}
			return token{token_type::integer, std::move(content)};
		}
		return boost::none;
	}
}

#endif
