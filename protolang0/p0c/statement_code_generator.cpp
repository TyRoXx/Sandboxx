#include "statement_code_generator.hpp"
#include "symbol_table.hpp"
#include "expression_code_generator.hpp"
#include "compiler_error.hpp"


namespace p0
{
	statement_code_generator::statement_code_generator(
		code_generator &function_generator,
		intermediate::emitter &emitter,
		symbol_table &symbols
		)
		: m_function_generator(function_generator)
		, m_emitter(emitter)
		, m_symbols(symbols)
	{
	}

	void statement_code_generator::visit(declaration_tree const &statement)
	{
		if (!m_symbols.add_symbol(
			source_range_to_string(statement.name()),
			symbol()
			))
		{
			throw compiler_error(
				"Name of local variable is already in use",
				statement.name()
				);
		}
	}

	void statement_code_generator::visit(return_tree const &statement)
	{
		m_emitter.return_();
	}

	void statement_code_generator::visit(block_tree const &statement)
	{
		symbol_table block_symbols(&m_symbols);

		for (auto s = begin(statement.body()); s != end(statement.body()); ++s)
		{
			generate_statement(
				**s,
				m_function_generator,
				m_emitter,
				block_symbols
				);
		}
	}

	void statement_code_generator::visit(expression_statement_tree const &statement)
	{
		generate_expression(
			statement.expression(),
			m_function_generator,
			m_emitter,
			reference(),
			m_symbols
			);
	}

	void statement_code_generator::visit(assignment_tree const &statement)
	{
	}


	void generate_statement(
		statement_tree const &tree,
		code_generator &function_generator,
		intermediate::emitter &emitter,
		symbol_table &symbols
		)
	{
		statement_code_generator generator(
			function_generator,
			emitter,
			symbols
			);
		tree.accept(generator);
	}
}
