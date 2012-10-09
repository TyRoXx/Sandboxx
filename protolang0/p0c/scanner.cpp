#include "scanner.hpp"
#include "compiler_error.hpp"
#include <cctype>


namespace p0
{
	namespace
	{
		bool is_whitespace(char c)
		{
			switch (c)
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				return true;

			default:
				return false;
			}
		}

		bool is_identifer_first(char c)
		{
			c = std::tolower(c);
			return
				((c >= 'a') && (c <= 'z')) ||
				(c == '_');
		}

		bool is_digit_10(char c)
		{
			return ((c >= '0') && (c <= '9'));
		}

		bool is_identifier_tail(char c)
		{
			return
				is_identifer_first(c) ||
				is_digit_10(c);
		}

		bool is_keyword(source_range content, char const *keyword)
		{
			for (auto i = content.begin(); i != content.end(); ++i)
			{
				if (*i != *keyword)
				{
					return false;
				}
			}

			return (*keyword == '\0');
		}

		struct keyword
		{
			char const *content;
			token_type::Enum token;
		};

		token_type::Enum find_keyword(source_range content)
		{
			static std::array<keyword, 3> const keywords =
			{{
				{"var", token_type::var},
				{"function", token_type::function},
				{"return", token_type::return_},
			}};

			using namespace std;

			for (auto k = begin(keywords); k != end(keywords); ++k)
			{
				if (is_keyword(content, k->content))
				{
					return k->token;
				}
			}

			return token_type::identifier;
		}
	}


	scanner::scanner(
		source_range source
		)
		: m_pos(source.begin())
		, m_end(source.end())
	{
	}

	token scanner::next_token()
	{
		if (m_pos == m_end)
		{
			return token(token_type::end_of_file, source_range(m_end, m_end));
		}

		skip_whitespace();

		switch (*m_pos)
		{
		case '(': return eat_single_char_token(token_type::parenthesis_left);
		case ')': return eat_single_char_token(token_type::parenthesis_right);

		default:
			if (is_identifer_first(*m_pos))
			{
				auto const identifier_begin = m_pos;
				do
				{
					++m_pos;
				}
				while (
					(m_pos != m_end) &&
					is_identifier_tail(*m_pos));
				auto const identifier_end = m_pos;

				source_range const range(
					identifier_begin,
					identifier_end
					);

				auto const type = find_keyword(range);
				
				return token(
					type,
					range
					);
			}

			if (is_digit_10(*m_pos))
			{
				auto const integer_begin = m_pos;
				do
				{
					++m_pos;
				}
				while (
					(m_pos != m_end) &&
					is_digit_10(*m_pos));
				auto const integer_end = m_pos;

				return token(
					token_type::integer_10,
					source_range(integer_begin, integer_end)
					);
			}

			throw compiler_error(
				"Unexpected character",
				source_range(m_pos, m_pos + 1)
				);
		}
	}

	void scanner::skip_line()
	{
		while (
			(m_pos != m_end) &&
			(*m_pos != '\n'))
		{
			++m_pos;
		}
	}


	void scanner::skip_whitespace()
	{
		while (m_pos != m_end)
		{
			if (is_whitespace(*m_pos))
			{
				++m_pos;
				continue;
			}

			if (*m_pos == ';')
			{
				do
				{
					++m_pos;
				}
				while (
					(m_pos != m_end) &&
					(*m_pos != '\n')
					);
				continue;
			}

			break;
		}
	}

	token scanner::eat_single_char_token(token_type::Enum type)
	{
		++m_pos;
		return token(
			type,
			source_range((m_pos - 1), m_pos)
			);
	}
}
