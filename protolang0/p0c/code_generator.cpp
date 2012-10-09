#include "code_generator.hpp"
#include "tree.hpp"


namespace p0
{
	code_generator::code_generator(
		unit_tree const &tree
		)
		: m_tree(tree)
	{
	}

	intermediate::unit code_generator::generate_unit()
	{
		return intermediate::unit("");
	}
}
