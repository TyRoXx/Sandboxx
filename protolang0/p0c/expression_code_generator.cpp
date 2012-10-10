#include "expression_code_generator.hpp"


namespace p0
{
	expression_code_generator::expression_code_generator(
		intermediate::emitter &emitter,
		symbol_table &symbols
		)
		: m_emitter(emitter)
		, m_symbols(symbols)
	{
	}

	void expression_code_generator::visit(name_expression_tree const &expression)
	{
	}

	void expression_code_generator::visit(integer_10_expression_tree const &expression)
	{
	}

	void expression_code_generator::visit(call_expression_tree const &expression)
	{
	}

	void expression_code_generator::visit(function_tree const &expression)
	{
	}

	void expression_code_generator::visit(null_expression_tree const &expression)
	{
		m_emitter.copy(0, 0);
	}


	void generate_expression(
		expression_tree const &tree,
		intermediate::emitter &emitter,
		symbol_table &symbols
		)
	{
		expression_code_generator generator(
			emitter,
			symbols
			);
		tree.accept(generator);
	}
}
