#pragma once
#ifndef P0C_PARSER_HPP
#define P0C_PARSER_HPP


#include "tree.hpp"


namespace p0
{
	struct scanner;


	struct parser
	{
		explicit parser(
			scanner &scanner
			);
		unit_tree parse_unit();

	private:

		scanner &m_scanner;
	};
}


#endif
