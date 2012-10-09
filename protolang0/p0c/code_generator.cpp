#include "code_generator.hpp"
#include "expression_tree.hpp"
#include "statement_tree.hpp"
#include "p0i/emitter.hpp"


namespace p0
{
	code_generator::code_generator(
		expression_tree const &tree
		)
		: m_tree(tree)
	{
	}

	intermediate::unit code_generator::generate_unit()
	{
		//TODO

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

		//TODO
		size_t parameters = 0;

		m_functions[function_index] = intermediate::function(
			std::move(instructions),
			parameters
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
				code_generator &code_generator,
				intermediate::emitter &emitter
				)
				: m_code_generator(code_generator)
				, m_emitter(emitter)
			{
			}

			virtual void visit(declaration_tree const &statement) override
			{
			}

			virtual void visit(return_tree const &statement) override
			{
				m_emitter.return_();
			}

			virtual void visit(block_tree const &statement) override
			{
				for (auto s = begin(statement.body()); s != end(statement.body()); ++s)
				{
					m_code_generator.generate_statement(
						**s,
						m_emitter
						);
				}
			}

			virtual void visit(expression_statement_tree const &statement) override
			{
				m_code_generator.generate_expression(
					statement.expression(),
					m_emitter
					);
			}

		private:

			code_generator &m_code_generator;
			intermediate::emitter &m_emitter;
		};

		statement_generator visitor(
			*this,
			emitter
			);
		statement_tree.accept(visitor);
	}

	void code_generator::generate_expression(
		expression_tree const &expression_tree,
		intermediate::emitter &emitter
		)
	{
		struct expression_generator : expression_tree_visitor
		{
			explicit expression_generator(
				code_generator &code_generator,
				intermediate::emitter &emitter
				)
				: m_code_generator(code_generator)
				, m_emitter(emitter)
			{
			}

			virtual void visit(name_expression_tree const &expression) override
			{
			}

			virtual void visit(integer_10_expression_tree const &expression) override
			{
			}

			virtual void visit(call_expression_tree const &expression) override
			{
			}

			virtual void visit(function_tree const &expression) override
			{
				m_code_generator.generate_function(expression);
			}

		private:

			code_generator &m_code_generator;
			intermediate::emitter &m_emitter;
		};

		expression_generator visitor(
			*this,
			emitter
			);
		expression_tree.accept(visitor);
	}
}
