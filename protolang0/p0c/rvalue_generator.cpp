#include "rvalue_generator.hpp"
#include "symbol_table.hpp"
#include "compiler_error.hpp"
#include "code_generator.hpp"


namespace p0
{
	rvalue_generator::rvalue_generator(
		code_generator &function_generator,
		intermediate::emitter &emitter,
		symbol_table &symbols,
		reference destination
		)
		: m_function_generator(function_generator)
		, m_emitter(emitter)
		, m_symbols(symbols)
		, m_destination(destination)
	{
	}
	

	void rvalue_generator::visit(name_expression_tree const &expression)
	{
		auto const address = m_symbols.require_symbol(
			expression.name()
			);
		
		if (m_destination.is_valid())
		{
			m_emitter.copy(
				m_destination.local_address(),
				address.local_address()
				);
		}
	}

	void rvalue_generator::visit(integer_10_expression_tree const &expression)
	{
		enum
		{
			base = 10,
		};

		intermediate::instruction_argument value = 0;

		//TODO: handle too large numbers

		auto const value_string = expression.value();
		for (auto i = value_string.begin(); i != value_string.end(); ++i)
		{
			value *= base;
			value += (*i - '0');
		}

		if (m_destination.is_valid())
		{
			m_emitter.set_from_constant(
				m_destination.local_address(),
				value
			);
		}
	}

	void rvalue_generator::visit(call_expression_tree const &expression)
	{
		auto const function_address = reference(66); //TODO

		rvalue_generator function(
			m_function_generator,
			m_emitter,
			m_symbols,
			function_address
			);
		expression.function().accept(function);

		size_t argument_address = 77; //TODO

		for (auto arg = expression.arguments().begin(), end = expression.arguments().end();
			arg != end; ++arg)
		{
			rvalue_generator argument(
				m_function_generator,
				m_emitter,
				m_symbols,
				reference(argument_address)
				);
			(*arg)->accept(argument);

			++argument_address;
		}

		m_emitter.call(
			expression.arguments().size()
			);

		if (m_destination.is_valid())
		{
			m_emitter.copy(
				m_destination.local_address(),
				76 //TODO
				);
		}
	}

	void rvalue_generator::visit(function_tree const &expression)
	{
		auto const function_id = m_function_generator.generate_function(
			expression
			);

		if (m_destination.is_valid())
		{
			m_emitter.set_function(
				m_destination.local_address(),
				function_id
				);
		}
	}

	void rvalue_generator::visit(null_expression_tree const &expression)
	{
		if (m_destination.is_valid())
		{
			m_emitter.set_null(
				m_destination.local_address()
				);
		}
	}
}
