#include "parser.hpp"
#include "syntax_error.hpp"
#include "scanner.hpp"
#include "lexical_error.hpp"


namespace p0
{
	parser::parser(
		scanner &scanner
		)
		: m_scanner(scanner)
	{
	}

	unit_tree parser::parse_unit()
	{
		return unit_tree();
	}
}
