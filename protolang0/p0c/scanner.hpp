#pragma once
#ifndef P0C_SCANNER_HPP
#define P0C_SCANNER_HPP


#include "p0i/unit.hpp"
#include "source_range.hpp"
#include "token.hpp"


namespace p0
{
	struct scanner
	{
		explicit scanner(
			source_range source
			);
		token next_token();
		void skip_line();

	private:

		source_range::iterator m_pos, m_end;


		void skip_whitespace();
		token eat_single_char_token(token_type::Enum type);
	};
}


#endif
