#ifndef FCT_PARSER_HPP
#define FCT_PARSER_HPP


#include "program/tree.hpp"
#include "scanner.hpp"


namespace fct
{
	struct Parser
	{
		static Tree parse(
			const TokenSequence &tokens
			);
	};
}


#endif