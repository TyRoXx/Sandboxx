#include "lvalue_generator.hpp"
#include "local_frame.hpp"
#include "compiler_error.hpp"
#include "temporary.hpp"
#include "rvalue_generator.hpp"
#include "code_generator.hpp"


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

	void lvalue_generator::commit_write()
	{
		if (m_commit_write)
		{
			m_commit_write();
		}
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

	void lvalue_generator::visit(dot_element_expression_tree const &expression)
	{
		auto const table_variable = std::make_shared<temporary>(
			std::ref(m_frame),
			1
			);

		rvalue_generator table_generator(
			m_function_generator,
			m_emitter,
			m_frame,
			table_variable->address()
			);
		expression.table().accept(table_generator);

		auto const key_variable = std::make_shared<temporary>(
			std::ref(m_frame),
			1
			);

		auto key = source_range_to_string(expression.element_name());
		auto const key_string_id = m_function_generator.get_string_id(
			std::move(key)
			);

		m_emitter.set_string(
			key_variable->address().local_address(),
			key_string_id
			);

		auto const element_variable = std::make_shared<temporary>(
			std::ref(m_frame),
			1
			);

		m_address = element_variable->address();

		auto &emitter = m_emitter;

		m_commit_write = [table_variable, key_variable, element_variable, &emitter]()
		{
			emitter.set_element(
				table_variable->address().local_address(),
				key_variable->address().local_address(),
				element_variable->address().local_address()
				);
		};
	}
}
