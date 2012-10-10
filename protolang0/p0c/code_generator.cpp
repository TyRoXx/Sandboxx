#include "code_generator.hpp"
#include "expression_tree.hpp"
#include "statement_tree.hpp"
#include "symbol_table.hpp"
#include "compiler_error.hpp"
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


	void code_generator::generate_function(
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
			emitter,
			parameter_symbols
			);
		
		m_functions[function_index] = intermediate::function(
			std::move(instructions),
			function.parameters().size()
			);
	}

	void code_generator::generate_statement(
		statement_tree const &statement_tree,
		intermediate::emitter &emitter,
		symbol_table &symbols
		)
	{
		struct statement_generator : statement_tree_visitor
		{
			explicit statement_generator(
				code_generator &code_generator,
				intermediate::emitter &emitter,
				symbol_table &symbols
				)
				: m_code_generator(code_generator)
				, m_emitter(emitter)
				, m_symbols(symbols)
			{
			}

			virtual void visit(declaration_tree const &statement) override
			{
				m_symbols.add_symbol(
					source_range_to_string(statement.name()),
					symbol()
					);
			}

			virtual void visit(return_tree const &statement) override
			{
				m_emitter.return_();
			}

			virtual void visit(block_tree const &statement) override
			{
				symbol_table block_symbols(&m_symbols);

				for (auto s = begin(statement.body()); s != end(statement.body()); ++s)
				{
					m_code_generator.generate_statement(
						**s,
						m_emitter,
						block_symbols
						);
				}
			}

			virtual void visit(expression_statement_tree const &statement) override
			{
				m_code_generator.generate_expression(
					statement.expression(),
					m_emitter,
					m_symbols
					);
			}

			virtual void visit(assignment_tree const &statement) override
			{
			}

		private:

			code_generator &m_code_generator;
			intermediate::emitter &m_emitter;
			symbol_table &m_symbols;
		};

		statement_generator visitor(
			*this,
			emitter,
			symbols
			);
		statement_tree.accept(visitor);
	}

	void code_generator::generate_expression(
		expression_tree const &expression_tree,
		intermediate::emitter &emitter,
		symbol_table &symbols
		)
	{
		struct expression_generator : expression_tree_visitor
		{
			explicit expression_generator(
				code_generator &code_generator,
				intermediate::emitter &emitter,
				symbol_table &symbols
				)
				: m_code_generator(code_generator)
				, m_emitter(emitter)
				, m_symbols(symbols)
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

			virtual void visit(null_expression_tree const &expression) override
			{
				m_emitter.copy(0, 0);
			}

		private:

			code_generator &m_code_generator;
			intermediate::emitter &m_emitter;
			symbol_table &m_symbols;
		};

		expression_generator visitor(
			*this,
			emitter,
			symbols
			);
		expression_tree.accept(visitor);
	}
}
