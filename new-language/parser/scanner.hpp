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

	struct character_position
	{
		std::size_t line, column;

		character_position()
			: line(0)
			, column(0)
		{
		}

		character_position(std::size_t line, std::size_t column)
			: line(line)
			, column(column)
		{
		}
	};

	inline bool operator == (character_position const &left, character_position const &right)
	{
		return (left.line == right.line) && (left.column == right.column);
	}

	struct token
	{
		token_type type;
		std::string content;
		character_position begin;
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

	struct source_char
	{
		char code_unit;
		character_position where;
	};

	inline bool operator == (source_char const &left, source_char const &right)
	{
		return (left.code_unit == right.code_unit) && (left.where == right.where);
	}

	inline boost::optional<token> scan_token(Si::source<source_char> &input)
	{
		auto const first = Si::get(input);
		if (!first)
		{
			return token{token_type::end_of_file, "", character_position{}};
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
		if (first->code_unit == '"')
		{
			std::string content;
			for (;;)
			{
				auto const next = Si::get(input);
				if (next->code_unit == '"')
				{
					return token{token_type::string, std::move(content), first->where};
				}
				switch (next->code_unit)
				{
				case '\\':
					{
						auto const escaped = Si::get(input);
						if (!escaped)
						{
							return boost::none;
						}
						switch (escaped->code_unit)
						{
						case '\\':
						case '"':
							content.push_back(escaped->code_unit);
							break;

						default:
							return boost::none;
						}
						break;
					}

				default:
					content.push_back(next->code_unit);
					break;
				}
			}
		}
		{
			auto const single_char_found = tokens.find(first->code_unit);
			if (single_char_found != end(tokens))
			{
				return token{single_char_found->second, std::string(1, first->code_unit), first->where};
			}
		}
		if (is_identifier_head(first->code_unit))
		{
			std::string content(1, first->code_unit);
			for (;;)
			{
				auto const next = peek(input);
				if (!next)
				{
					break;
				}
				if (!is_identifier_middle(next->code_unit))
				{
					break;
				}
				content.push_back(next->code_unit);
				Si::get(input);
			}
			if (content == "return")
			{
				return token{token_type::return_, std::move(content), first->where};
			}
			return token{token_type::identifier, std::move(content), first->where};
		}
		else if (is_decimal_digit(first->code_unit))
		{
			std::string content(1, first->code_unit);
			for (;;)
			{
				auto const next = peek(input);
				if (!next)
				{
					break;
				}
				if (!is_decimal_digit(next->code_unit))
				{
					break;
				}
				content.push_back(next->code_unit);
				Si::get(input);
			}
			return token{token_type::integer, std::move(content), first->where};
		}
		return boost::none;
	}
}

#endif
