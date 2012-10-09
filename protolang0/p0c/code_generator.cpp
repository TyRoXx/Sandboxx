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
		function_tree const &function
		)
	{
		//reserve a function index for later insertion
		auto const function_index = m_functions.size();
		m_functions.resize(function_index + 1);

		intermediate::function::instruction_vector instructions;

		m_functions[function_index] = intermediate::function(
			std::move(instructions)
			);
	}

	void code_generator::generate_statement(
		statement_tree const &statement_tree,
		intermediate::function::instruction_vector &instructions
		)
	{
		struct statement_generator : statement_tree_visitor
		{
			explicit statement_generator(
				intermediate::function::instruction_vector &instructions
				)
				: m_instructions(instructions)
			{
			}

			virtual void visit(declaration_tree const &statement) const override
			{
			}

			virtual void visit(return_tree const &statement) const override
			{
			}

			virtual void visit(block_tree const &statement) const override
			{
			}

		private:

			intermediate::function::instruction_vector &m_instructions;
		};

		statement_generator visitor(instructions);
		statement_tree.accept(visitor);
	}
}
