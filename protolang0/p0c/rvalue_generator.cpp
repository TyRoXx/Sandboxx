#include "rvalue_generator.hpp"
#include "local_frame.hpp"
#include "compiler_error.hpp"
#include "code_generator.hpp"
#include "temporary.hpp"


namespace p0
{
	rvalue_generator::rvalue_generator(
		code_generator &function_generator,
		intermediate::emitter &emitter,
		local_frame &frame,
		reference destination
		)
		: m_function_generator(function_generator)
		, m_emitter(emitter)
		, m_frame(frame)
		, m_destination(destination)
	{
	}
	

	void rvalue_generator::visit(name_expression_tree const &expression)
	{
		auto const address = m_frame.require_symbol(
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

	namespace
	{
		void decode_string_literal(
			std::string &decoded,
			source_range literal
			)
		{
			for (auto i = literal.begin(); i != literal.end(); ++i)
			{
				auto c = *i;

				assert(c != '"'); //scanner guarantees this

				switch (c)
				{
				case '\\':
					{
						++i;
						assert(i != literal.end());

						auto const second = *i;

						switch (second)
						{
						case 't': c = '\t'; break;
						case 'n': c = '\n'; break;
						case 'r': c = '\r'; break;

						case '\\':
						case '\'':
						case '\"':
							c = second;
							break;

						default:
							throw compiler_error(
								"Invalid escape sequence",
								source_range(i - 1, i + 1)
								);
						}
						break;
					}

				default:
					break;
				}

				decoded += c;
			}
		}
	}

	void rvalue_generator::visit(string_expression_tree const &expression)
	{
		std::string value;

		for (auto p = expression.parts().begin(), end = expression.parts().end();
			p != end; ++p)
		{
			decode_string_literal(
				value,
				*p
				);
		}

		if (m_destination.is_valid())
		{
			auto const string_id = m_function_generator.get_string_id(
				std::move(value)
				);

			m_emitter.set_string(
				m_destination.local_address(),
				string_id
				);
		}
	}

	void rvalue_generator::visit(call_expression_tree const &expression)
	{
		temporary const function_variable(
			m_frame,
			1
			);

		try
		{
			rvalue_generator function(
				m_function_generator,
				m_emitter,
				m_frame,
				function_variable.address()
				);
			expression.function().accept(function);
		}
		catch (compiler_error const &e)
		{
			m_function_generator.handle_error(e);
		}

		temporary const result_variable(
			m_frame,
			1
			);

		auto const argument_count = expression.arguments().size();
		temporary const argument_variables(
			m_frame,
			argument_count
			);
		size_t current_argument_address = argument_variables.address().local_address();

		for (auto arg = expression.arguments().begin(), end = expression.arguments().end();
			arg != end; ++arg, ++current_argument_address)
		{
			try
			{
				rvalue_generator argument(
					m_function_generator,
					m_emitter,
					m_frame,
					reference(current_argument_address)
					);
				(*arg)->accept(argument);
			}
			catch (compiler_error const &e)
			{
				m_function_generator.handle_error(e);
			}
		}

		m_emitter.call(
			expression.arguments().size()
			);

		if (m_destination.is_valid())
		{
			m_emitter.copy(
				m_destination.local_address(),
				result_variable.address().local_address()
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

	void rvalue_generator::visit(table_expression const &expression)
	{
		auto const table_address = m_destination;

		if (table_address.is_valid())
		{
			m_emitter.new_table(
				table_address.local_address()
				);
		}

		for (auto element = expression.elements().begin(), end = expression.elements().end();
			element != end; ++element)
		{
			auto const &value = *element->second;

			try
			{
				if (table_address.is_valid())
				{
					source_range const key = element->first;

					temporary const key_variable(
						m_frame,
						1
						);

					{
						auto const key_string_id = m_function_generator.get_string_id(
							source_range_to_string(key)
							);

						m_emitter.set_string(
							key_variable.address().local_address(),
							key_string_id
							);
					}

					temporary const value_variable(
						m_frame,
						1
						);

					rvalue_generator value_generator(
						m_function_generator,
						m_emitter,
						m_frame,
						value_variable.address()
						);
					value.accept(value_generator);

					m_emitter.set_element(
						table_address.local_address(),
						key_variable.address().local_address(),
						value_variable.address().local_address()
						);
				}
				else
				{
					rvalue_generator value_generator(
						m_function_generator,
						m_emitter,
						m_frame,
						reference() //ignore value
						);
					value.accept(value_generator);
				}
			}
			catch (compiler_error const &e)
			{
				m_function_generator.handle_error(e);
			}
		}
	}

	void rvalue_generator::visit(unary_expression_tree const &expression)
	{
		rvalue_generator input_generator(
			m_function_generator,
			m_emitter,
			m_frame,
			m_destination
			);
		expression.input().accept(input_generator);

		if (m_destination.is_valid())
		{
			switch (expression.type())
			{
			case unary_operator::not_:
				m_emitter.not_(m_destination.local_address());
				break;

			case unary_operator::inverse:
				m_emitter.invert(m_destination.local_address());
				break;

			case unary_operator::negative:
				m_emitter.negate(m_destination.local_address());
				break;
			}
		}
	}

	void rvalue_generator::visit(dot_element_expression_tree const &expression)
	{
		auto const table_address = m_destination;
		auto const value_address = m_destination;

		rvalue_generator table_generator(
			m_function_generator,
			m_emitter,
			m_frame,
			table_address
			);
		expression.table().accept(table_generator);

		temporary const key_variable(
			m_frame,
			1
			);

		auto const key_address = key_variable.address();

		auto key = source_range_to_string(expression.element_name());
		auto const key_string_id = m_function_generator.get_string_id(
			std::move(key)
			);

		m_emitter.set_string(
			key_address.local_address(),
			key_string_id
			);

		if (m_destination.is_valid())
		{
			m_emitter.get_element(
				table_address.local_address(),
				key_address.local_address(),
				value_address.local_address()
				);
		}
	}
}
