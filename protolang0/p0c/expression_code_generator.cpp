#include "expression_code_generator.hpp"
#include "p0i/instruction.hpp"


namespace p0
{
	expression_code_generator::expression_code_generator(
		intermediate::emitter &emitter,
		reference destination,
		symbol_table &symbols
		)
		: m_emitter(emitter)
		, m_destination(destination)
		, m_symbols(symbols)
	{
	}

	void expression_code_generator::visit(name_expression_tree const &expression)
	{
	}

	void expression_code_generator::visit(integer_10_expression_tree const &expression)
	{
		if (!m_destination.is_valid())
		{
			return;
		}

		auto const value_string = expression.value();
		intermediate::instruction_argument value = 0;
		for (auto i = value_string.begin(); i != value_string.end(); ++i)
		{
			value *= 10;
			value += (*i - '0');
		}

		m_emitter.set_from_constant(
			m_destination.local_address(),
			value
			);
	}

	void expression_code_generator::visit(call_expression_tree const &expression)
	{
	}

	void expression_code_generator::visit(function_tree const &expression)
	{
	}

	void expression_code_generator::visit(null_expression_tree const &expression)
	{
		if (!m_destination.is_valid())
		{
			return;
		}

		m_emitter.set_null(
			m_destination.local_address()
			);
	}


	void generate_expression(
		expression_tree const &tree,
		intermediate::emitter &emitter,
		reference destination,
		symbol_table &symbols
		)
	{
		expression_code_generator generator(
			emitter,
			destination,
			symbols
			);
		tree.accept(generator);
	}
}
