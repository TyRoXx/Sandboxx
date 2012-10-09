#include "code_generator.hpp"
#include "function_tree.hpp"
#include "p0i/emitter.hpp"


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
		intermediate::emitter emitter(instructions);
		generate_statement(
			function.body(),
			emitter
			);

		m_functions[function_index] = intermediate::function(
			std::move(instructions)
			);
	}

	void code_generator::generate_statement(
		statement_tree const &statement_tree,
		intermediate::emitter &emitter
		)
	{
		struct statement_generator : statement_tree_visitor
		{
			explicit statement_generator(
				intermediate::emitter &emitter
				)
				: m_emitter(emitter)
			{
			}

			virtual void visit(declaration_tree const &statement) const override
			{
			}

			virtual void visit(return_tree const &statement) const override
			{
				m_emitter.return_();
			}

			virtual void visit(block_tree const &statement) const override
			{
			}

		private:

			intermediate::emitter &m_emitter;
		};

		statement_generator visitor(emitter);
		statement_tree.accept(visitor);
	}
}
