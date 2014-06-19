#ifndef NEW_LANG_AST_HPP
#define NEW_LANG_AST_HPP

#include "scanner.hpp"
#include <boost/variant.hpp>

namespace nl
{
	namespace ast
	{
		struct identifier
		{
			token position;
		};

		struct string
		{
			token position;
		};

		struct integer
		{
			token position;
		};

		struct lambda;
		struct subscript;
		struct call;

		typedef boost::variant<identifier, string, integer, boost::recursive_wrapper<lambda>, boost::recursive_wrapper<subscript>, boost::recursive_wrapper<call>> expression;

		struct definition;

		struct block
		{
			std::vector<definition> elements;
			expression result;
		};

		struct parameter;

		struct lambda
		{
			std::vector<parameter> parameters;
			boost::optional<expression> explicit_return_type;
			block body;
		};

		struct subscript
		{
			expression left;
			token element;
		};

		struct call
		{
			expression function;
			std::vector<expression> arguments;
		};

		struct parameter
		{
			expression type;
			token name;
		};

		struct definition
		{
			token name;
			expression value;
		};

		void print(Si::sink<char> &out, expression const &value, std::size_t indentation);

		inline void print_indentation(Si::sink<char> &out, std::size_t indentation)
		{
			for (size_t i = 0; i < indentation; ++i)
			{
				Si::append(out, "\t");
			}
		}

		struct expression_printer : boost::static_visitor<>
		{
			explicit expression_printer(Si::sink<char> &out, std::size_t indentation)
				: m_out(out)
				, m_indentation(indentation)
			{
			}

			void operator()(identifier const &value) const
			{
				Si::append(m_out, value.position.content);
			}

			void operator()(string const &value) const
			{
				Si::append(m_out, "\"");
				for (auto c : value.position.content)
				{
					switch (c)
					{
					case '\"':
						Si::append(m_out, "\\\"");
						break;

					case '\\':
						Si::append(m_out, "\\\\");
						break;

					default:
						Si::append(m_out, c);
						break;
					}
				}
				Si::append(m_out, "\"");
			}

			void operator()(integer const &value) const
			{
				Si::append(m_out, value.position.content);
			}

			void operator()(lambda const &value) const
			{
				Si::append(m_out, "(");
				for (auto p = begin(value.parameters); p != end(value.parameters); ++p)
				{
					auto const &parameter = *p;
					print(m_out, parameter.type, m_indentation);
					Si::append(m_out, " ");
					Si::append(m_out, parameter.name.content);
					if (p != (end(value.parameters) - 1))
					{
						Si::append(m_out, ", ");
					}
				}
				Si::append(m_out, ")\n");
				for (auto &definition : value.body.elements)
				{
					print_indentation(m_out, m_indentation + 1);
					Si::append(m_out, definition.name.content);
					Si::append(m_out, " = ");
					print(m_out, definition.value, m_indentation + 1);
					Si::append(m_out, "\n");
				}
				print_indentation(m_out, m_indentation + 1);
				Si::append(m_out, "return ");
				print(m_out, value.body.result, m_indentation + 1);
				Si::append(m_out, "\n");
			}

			void operator()(subscript const &value) const
			{
				print(m_out, value.left, m_indentation);
				Si::append(m_out, ".");
				Si::append(m_out, value.element.content);
			}

			void operator()(call const &value) const
			{
				print(m_out, value.function, m_indentation);
				Si::append(m_out, "(");
				for (auto a = begin(value.arguments); a != end(value.arguments); ++a)
				{
					print(m_out, *a, m_indentation);
					if (a != (end(value.arguments) - 1))
					{
						Si::append(m_out, ", ");
					}
				}
				Si::append(m_out, ")");
			}

		private:

			Si::sink<char> &m_out;
			std::size_t m_indentation;
		};

		inline void print(Si::sink<char> &out, expression const &value, std::size_t indentation)
		{
			return boost::apply_visitor(expression_printer(out, indentation), value);
		}

		inline bool operator == (expression const &left, expression const &right)
		{
			std::vector<char> left_str, right_str;
			auto left_sink = Si::make_container_sink(left_str);
			auto right_sink = Si::make_container_sink(right_str);
			print(left_sink, left, 0);
			print(right_sink, right, 0);
			return (left_str == right_str);
		}

		inline std::ostream &operator << (std::ostream &out, expression const &value)
		{
			Si::ostream_ref_sink sink(out);
			print(sink, value, 0);
			return out;
		}
	}
}

#endif
