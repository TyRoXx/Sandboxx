#include "scanner.hpp"
#include "lexical_error.hpp"
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

				return token(
					token_type::identifier,
					source_range(identifier_begin, identifier_end)
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

			throw lexical_error(
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
					(*m_pos != '\n'));
				continue;
			}

			break;
		}
	}
}
