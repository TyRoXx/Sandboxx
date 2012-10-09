#include "code_generator.hpp"
#include "function_tree.hpp"


namespace p0
{
	code_generator::code_generator(
		function_tree const &tree
		)
		: m_tree(tree)
	{
	}

	intermediate::unit code_generator::generate_unit()
	{
		generate_function(m_tree);

		return intermediate::unit(
			std::move(m_functions)
			);
	}


	void code_generator::generate_function(
		const function_tree &function
		)
	{
		intermediate::function::instruction_vector instructions;

		//reserve a function index for later insertion
		auto const function_index = m_functions.size();
		m_functions.resize(function_index + 1);

		for (auto s = begin(function.body()); s != end(function.body()); ++s)
		{
		}

		m_functions[function_index] = intermediate::function(
			std::move(instructions)
			);
	}
}
