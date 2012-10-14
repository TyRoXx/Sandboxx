#include "lvalue_generator.hpp"
#include "local_frame.hpp"
#include "compiler_error.hpp"


namespace p0
{
	lvalue_generator::lvalue_generator(
		code_generator &function_generator,
		intermediate::emitter &emitter,
		local_frame &frame
		)
		: m_function_generator(function_generator)
		, m_emitter(emitter)
		, m_frame(frame)
	{
	}

	reference lvalue_generator::address() const
	{
		return m_address;
	}


	void lvalue_generator::visit(name_expression_tree const &expression)
	{
		auto const address = m_frame.require_symbol(
			expression.name()
			);

		m_address = address;
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

	void lvalue_generator::visit(table_expression const &expression)
	{
		throw compiler_error(
			"A table literal is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(unary_expression_tree const &expression)
	{
		throw compiler_error(
			"A unary operator expression is not an LValue",
			expression.position()
			);
	}
}
