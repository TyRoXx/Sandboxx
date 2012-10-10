#include "code_generator.hpp"
#include "expression_tree.hpp"
#include "statement_tree.hpp"
#include "symbol_table.hpp"
#include "compiler_error.hpp"
#include "statement_code_generator.hpp"
#include "p0i/emitter.hpp"


namespace p0
{
	code_generator::code_generator(
		function_tree const &tree,
		compiler_error_handler error_handler
		)
		: m_tree(tree)
		, m_error_handler(std::move(error_handler))
	{
	}

	intermediate::unit code_generator::generate_unit()
	{
		generate_function(
			m_tree
			);

		return intermediate::unit(
			std::move(m_functions)
			);
	}

	size_t code_generator::generate_function(
		function_tree const &function
		)
	{
		//reserve a function index for later insertion
		auto const function_index = m_functions.size();
		m_functions.resize(function_index + 1);

		intermediate::function::instruction_vector instructions;
		intermediate::emitter emitter(instructions);

		symbol_table parameter_symbols(
			nullptr
			);
		for (auto p = begin(function.parameters()), e = end(function.parameters()); p != e; ++p)
		{
			auto name = source_range_to_string(*p);

			if (!parameter_symbols.add_symbol(
				std::move(name),
				symbol()
				))
			{
				m_error_handler(compiler_error(
					"Duplicated parameter name",
					*p
					));
			}
		}

		generate_statement(
			function.body(),
			*this,
			emitter,
			parameter_symbols
			);
		
		m_functions[function_index] = intermediate::function(
			std::move(instructions),
			function.parameters().size()
			);

		return function_index;
	}
}
