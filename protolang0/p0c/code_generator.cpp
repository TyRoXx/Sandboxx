#include "code_generator.hpp"
#include "expression_tree.hpp"
#include "statement_tree.hpp"
#include "local_frame.hpp"
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

		local_frame top_frame(
			nullptr
			);
		for (auto p = begin(function.parameters()), e = end(function.parameters()); p != e; ++p)
		{
			try
			{
				top_frame.declare_variable(*p);
			}
			catch (compiler_error const &e)
			{
				m_error_handler(e);
			}
		}

		generate_statement(
			function.body(),
			*this,
			emitter,
			top_frame
			);
		
		m_functions[function_index] = intermediate::function(
			std::move(instructions),
			function.parameters().size()
			);

		return function_index;
	}

	void code_generator::handle_error(
		compiler_error const &error
		)
	{
		m_error_handler(error);
	}
}
