#include "lvalue_generator.hpp"
#include "symbol_table.hpp"
#include "compiler_error.hpp"


namespace p0
{
	lvalue_generator::lvalue_generator(
		code_generator &function_generator,
		intermediate::emitter &emitter,
		symbol_table &symbols
		)
		: m_function_generator(function_generator)
		, m_emitter(emitter)
		, m_symbols(symbols)
	{
	}

	reference lvalue_generator::address() const
	{
		return m_address;
	}


	void lvalue_generator::visit(name_expression_tree const &expression)
	{
		reference const * const symbol = m_symbols.find_symbol(
			source_range_to_string(expression.name())
			);

		if (!symbol)
		{
			//TODO: handle these errors in one place
			throw compiler_error(
				"Unknown identifier",
				expression.position()
				);
		}

		m_address = *symbol;
	}

	void lvalue_generator::visit(integer_10_expression_tree const &expression)
	{
		throw compiler_error(
			"An integer literal is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(call_expression_tree const &expression)
	{
		throw compiler_error(
			"A function result is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(function_tree const &expression)
	{
		throw compiler_error(
			"A function is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(null_expression_tree const &expression)
	{
		throw compiler_error(
			"'null' is not an LValue",
			expression.position()
			);
	}
}
