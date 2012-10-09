#pragma once
#ifndef P0C_TOKEN_HPP
#define P0C_TOKEN_HPP


#include "source_range.hpp"


namespace p0
{
	struct token_type
	{
		enum Enum
		{
			identifier,
			integer_10,
			parenthesis_left,
			parenthesis_right,
			assign,
			end_of_file,
		};
	};


	struct token
	{
		token_type::Enum type;
		source_range content;


		token();
		explicit token(token_type::Enum type, source_range content);
	};
}


#endif
