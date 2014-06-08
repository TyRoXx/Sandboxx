#ifndef NEW_LANG_ANALYZE_HPP
#define NEW_LANG_ANALYZE_HPP

#include "ast.hpp"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lexical_cast.hpp>

namespace nl
{
	namespace il
	{
		struct map;
		struct signature;

		struct external
		{
			void *payload;
		};

		struct integer
		{
			std::string value;
		};

		struct string
		{
			std::string value;
		};

		struct null
		{
		};

		typedef boost::variant<null, boost::recursive_wrapper<map>, boost::recursive_wrapper<signature>, external, integer, string> value;
		typedef value type;

		struct map
		{
			boost::unordered_map<value, value> elements;
		};

		struct signature
		{
			type result;
			std::vector<type> parameters;
		};

		struct constant_expression
		{
			value constant;
		};

		struct definition_expression
		{
			std::string name;
			std::size_t hops;
		};

		struct make_closure;
		struct subscript;
		struct call;

		typedef boost::variant<
			constant_expression,
			boost::recursive_wrapper<make_closure>,
			boost::recursive_wrapper<subscript>,
			boost::recursive_wrapper<call>,
			definition_expression
		> expression;

		struct parameter
		{
			expression type;
			std::string name;
		};

		struct definition
		{
			std::string name;
			expression value;
		};

		struct block
		{
			std::vector<definition> definitions;
			expression result;
		};

		struct make_closure
		{
			std::vector<parameter> parameters;
			block body;
		};

		struct subscript
		{
			expression left;
			std::string element;
		};

		struct call
		{
			expression function;
			std::vector<expression> arguments;
		};

		struct name_space
		{
			name_space const *next;
			boost::unordered_set<std::string> definitions;
		};

		inline boost::optional<definition_expression> resolve_name(name_space const &leaf, std::string const &name)
		{
			name_space const *n = &leaf;
			std::size_t hops = 0;
			while (n)
			{
				auto definition = n->definitions.find(name);
				if (definition != end(n->definitions))
				{
					return definition_expression{name, hops};
				}
				n = n->next;
				++hops;
			}
			return boost::none;
		}

		bool is_callable(expression const &function, std::vector<expression> const &arguments)
		{
			throw std::logic_error("not implemented");
		}

		expression analyze(ast::expression const &syntax, name_space const &names);

		struct expression_analyzer : boost::static_visitor<expression>
		{
			explicit expression_analyzer(name_space const &names)
				: m_names(names)
			{
			}

			expression operator()(ast::integer const &syntax) const
			{
				return constant_expression{integer{syntax.position.content}};
			}

			expression operator()(ast::string const &syntax) const
			{
				return constant_expression{string{syntax.position.content}};
			}

			expression operator()(ast::identifier const &syntax) const
			{
				auto expr = resolve_name(m_names, syntax.position.content);
				if (!expr)
				{
					throw std::runtime_error("Unknown identifier " + syntax.position.content);
				}
				return std::move(*expr);
			}

			expression operator()(ast::lambda const &syntax) const
			{
				name_space locals{&m_names, {}};
				std::vector<parameter> parameters;
				for (ast::parameter const &parameter_syntax : syntax.parameters)
				{
					auto type_expr = analyze(parameter_syntax.type, locals);
					parameters.emplace_back(parameter{type_expr, parameter_syntax.name.content});
					if (!locals.definitions.insert(parameter_syntax.name.content).second)
					{
						throw std::runtime_error("Cannot redefine " + parameter_syntax.name.content);
					}
				}
				block body;
				for (ast::definition const &definition_syntax : syntax.body.elements)
				{
					auto value = analyze(definition_syntax.value, m_names);
					body.definitions.emplace_back(definition{definition_syntax.name.content, std::move(value)});
					if (!locals.definitions.insert(definition_syntax.name.content).second)
					{
						throw std::runtime_error("Cannot redefine " + definition_syntax.name.content);
					}
				}
				body.result = analyze(syntax.body.result, locals);
				return make_closure{std::move(parameters), std::move(body)};
			}

			expression operator()(ast::subscript const &syntax) const
			{
				auto left = analyze(syntax.left, m_names);
				return subscript{std::move(left), syntax.element.content};
			}

			expression operator()(ast::call const &syntax) const
			{
				auto function = analyze(syntax.function, m_names);
				std::vector<expression> arguments;
				for (auto &argument : syntax.arguments)
				{
					arguments.emplace_back(analyze(argument, m_names));
				}
				if (!is_callable(function, arguments))
				{
					throw std::runtime_error("Argument type mismatch");
				}
				return call{std::move(function), std::move(arguments)};
			}

		private:

			name_space const &m_names;
		};

		inline expression analyze(ast::expression const &syntax, name_space const &names)
		{
			return boost::apply_visitor(expression_analyzer{names}, syntax);
		}

		void print(Si::sink<char> &sink, value const &v);
		void print(Si::sink<char> &sink, expression const &expr);

		struct value_printer : boost::static_visitor<>
		{
			explicit value_printer(Si::sink<char> &out)
				: m_out(out)
			{
			}

			void operator()(null) const
			{
				Si::append(m_out, "-null-");
			}

			void operator()(map const &value) const
			{
				Si::append(m_out, "-map-[\n");
				for (auto &elem : value.elements)
				{
					print(m_out, elem.first);
					Si::append(m_out, ": ");
					print(m_out, elem.second);
					Si::append(m_out, "\n");
				}
				Si::append(m_out, "]");
			}

			void operator()(signature const &value) const
			{
				Si::append(m_out, "-signature-(");
				for (auto &param : value.parameters)
				{
					print(m_out, param);
					Si::append(m_out, ", ");
				}
				Si::append(m_out, ")->");
				print(m_out, value.result);
			}

			void operator()(external const &value) const
			{
				Si::append(m_out, boost::lexical_cast<std::string>(value.payload));
			}

			void operator()(integer const &value) const
			{
				Si::append(m_out, value.value);
			}

			void operator()(string const &value) const
			{
				Si::append(m_out, value.value);
			}

		private:

			Si::sink<char> &m_out;
		};

		inline void print(Si::sink<char> &sink, value const &v)
		{
			return boost::apply_visitor(value_printer{sink}, v);
		}

		struct expression_printer : boost::static_visitor<>
		{
			explicit expression_printer(Si::sink<char> &out)
				: m_out(out)
			{
			}

			void operator()(constant_expression const &expr) const
			{
				print(m_out, expr.constant);
			}

			void operator()(make_closure const &expr) const
			{
				Si::append(m_out, "(");
				for (parameter const &param : expr.parameters)
				{
					print(m_out, param.type);
					Si::append(m_out, " ");
					Si::append(m_out, param.name);
					Si::append(m_out, ", ");
				}
				Si::append(m_out, ")\n");
				for (definition const &def : expr.body.definitions)
				{
					Si::append(m_out, def.name);
					Si::append(m_out, " = ");
					print(m_out, def.value);
					Si::append(m_out, "\n");
				}
				print(m_out, expr.body.result);
				Si::append(m_out, "\n");
			}

			void operator()(subscript const &expr) const
			{
				print(m_out, expr.left);
				Si::append(m_out, ".");
				Si::append(m_out, expr.element);
			}

			void operator()(call const &expr) const
			{
				print(m_out, expr.function);
				Si::append(m_out, "(...)");
			}

			void operator()(definition_expression const &expr) const
			{
				Si::append(m_out, expr.name);
				Si::append(m_out, "(");
				Si::append(m_out, boost::lexical_cast<std::string>(expr.hops));
				Si::append(m_out, ")");
			}

		private:

			Si::sink<char> &m_out;
		};

		inline void print(Si::sink<char> &sink, expression const &expr)
		{
			return boost::apply_visitor(expression_printer{sink}, expr);
		}

		inline bool operator == (expression const &left, expression const &right)
		{
			std::string left_str, right_str;
			auto left_sink = Si::make_container_sink(left_str);
			auto right_sink = Si::make_container_sink(right_str);
			print(left_sink, left);
			print(right_sink, right);
			return (left_str == right_str);
		}

		inline std::ostream &operator << (std::ostream &out, expression const &expr)
		{
			Si::ostream_ref_sink sink(out);
			print(sink, expr);
			return out;
		}
	}
}

#endif
