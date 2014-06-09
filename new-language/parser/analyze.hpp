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
			void const *payload;
		};

		inline bool operator == (external const &left, external const &right)
		{
			return (left.payload == right.payload);
		}

		inline std::size_t hash_value(external const &value)
		{
			return boost::hash_value(value.payload);
		}

		struct integer
		{
			std::string value;
		};

		inline bool operator == (integer const &left, integer const &right)
		{
			return (left.value == right.value);
		}

		inline std::size_t hash_value(integer const &value)
		{
			return boost::hash_value(value.value);
		}

		struct string
		{
			std::string value;
		};

		inline bool operator == (string const &left, string const &right)
		{
			return (left.value == right.value);
		}

		inline std::size_t hash_value(string const &value)
		{
			return boost::hash_value(value.value);
		}

		struct null
		{
		};

		inline bool operator == (null, null)
		{
			return true;
		}

		inline std::size_t hash_value(null const &)
		{
			return boost::hash_value(true);
		}

		typedef boost::variant<null, boost::recursive_wrapper<map>, boost::recursive_wrapper<signature>, external, integer, string> value;
		typedef value type;

		struct map
		{
			boost::unordered_map<value, value> elements;
		};

		inline bool operator == (map const &left, map const &right)
		{
			return (left.elements == right.elements);
		}

		inline std::size_t hash_value(map const &value)
		{
			std::size_t digest = 0;
			for (auto &entry : value.elements)
			{
				boost::hash_combine(digest, entry);
			}
			return digest;
		}

		struct signature
		{
			type result;
			std::vector<type> parameters;
		};

		inline bool operator == (signature const &left, signature const &right)
		{
			return
					(left.result == right.result) &&
					(left.parameters == right.parameters);
		}

		inline std::size_t hash_value(signature const &value)
		{
			std::size_t digest = 0;
			boost::hash_combine(digest, value.result);
			boost::hash_combine(digest, value.parameters);
			return digest;
		}

		struct constant_expression
		{
			value constant;
		};

		inline bool operator == (constant_expression const &left, constant_expression const &right)
		{
			return (left.constant == right.constant);
		}

		enum class local
		{
			bound,
			argument,
			definition
		};

		struct local_identifier
		{
			local type;
			std::size_t index;
		};

		bool operator == (local_identifier const &left, local_identifier const &right)
		{
			return
					(left.type == right.type) &&
					(left.index == right.index);
		}

		struct local_expression
		{
			local_identifier which;
			il::type type;
			std::string name;
			boost::optional<value> const_value;
		};

		struct make_closure;
		struct subscript;
		struct call;

		bool operator == (make_closure const &left, make_closure const &right);
		bool operator == (subscript const &left, subscript const &right);
		bool operator == (call const &left, call const &right);
		bool operator == (local_expression const &left, local_expression const &right);

		typedef boost::variant<
			constant_expression,
			boost::recursive_wrapper<make_closure>,
			boost::recursive_wrapper<subscript>,
			boost::recursive_wrapper<call>,
			local_expression
		> expression;

		struct parameter
		{
			il::type type;
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
			std::vector<local_identifier> bind_from_parent;
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

		inline bool operator == (local_expression const &left, local_expression const &right)
		{
			return
					(left.which == right.which) &&
					(left.name == right.name) &&
					(left.type == right.type) &&
					(left.const_value == right.const_value);
		}

		inline bool operator == (block const &left, block const &right)
		{
			return
					(left.definitions == right.definitions) &&
					(left.result == right.result);
		}

		inline bool operator == (parameter const &left, parameter const &right)
		{
			return
					(left.type == right.type) &&
					(left.name == right.name);
		}

		inline bool operator == (make_closure const &left, make_closure const &right)
		{
			return
					(left.parameters == right.parameters) &&
					(left.body == right.body);
		}

		inline bool operator == (subscript const &left, subscript const &right)
		{
			return
					(left.left == right.left) &&
					(left.element == right.element);
		}

		inline bool operator == (call const &left, call const &right)
		{
			return
					(left.function == right.function) &&
					(left.arguments == right.arguments);
		}

		inline bool operator == (definition const &left, definition const &right)
		{
			return
					(left.name == right.name) &&
					(left.value == right.value);
		}

		struct name_space_entry
		{
			local_identifier which;
			il::type type;
			boost::optional<value> const_value;
		};

		struct name_space
		{
			name_space *next;
			boost::unordered_map<std::string, name_space_entry> definitions;
			std::vector<local_identifier> bind_from_parent;
		};

		inline boost::optional<local_expression> require_local_identifier(name_space &where, std::string const &symbol)
		{
			auto found = where.definitions.find(symbol);
			if (found != end(where.definitions))
			{
				return local_expression{found->second.which, found->second.type, symbol, found->second.const_value};
			}

			if (!where.next)
			{
				return boost::none;
			}

			boost::optional<local_expression> const in_parent = require_local_identifier(*where.next, symbol);
			if (!in_parent)
			{
				return boost::none;
			}

			std::size_t const bound_index = where.bind_from_parent.size();
			where.bind_from_parent.emplace_back(in_parent->which);
			return local_expression{local_identifier{local::bound, bound_index}, in_parent->type, in_parent->name, in_parent->const_value};
		}

		type type_of_expression(expression const &expr);

		struct subscription_visitor : boost::static_visitor<boost::optional<type>>
		{
			explicit subscription_visitor(std::string element)
				: m_element(std::move(element))
			{
			}

			boost::optional<type> operator()(map const &m) const
			{
				auto e = m.elements.find(string{m_element});
				if  (e == end(m.elements))
				{
					return boost::none;
				}
				return e->second;
			}

			template <class Rest>
			boost::optional<type> operator()(Rest const &) const
			{
				return boost::none;
			}

		private:

			std::string m_element;
		};

		struct expression_type_visitor : boost::static_visitor<type>
		{
			type operator()(constant_expression const &) const
			{
				return null();
			}

			type operator()(make_closure const &closure) const
			{
				signature sig;
				sig.result = type_of_expression(closure.body.result);
				for (parameter const &param : closure.parameters)
				{
					sig.parameters.emplace_back(param.type);
				}
				return std::move(sig);
			}

			type operator()(subscript const &expr) const
			{
				auto left = type_of_expression(expr.left);
				auto element_type = boost::apply_visitor(subscription_visitor{expr.element}, left);
				if (!element_type)
				{
					throw std::runtime_error("Cannot deduce type because element does not exist: " + expr.element);
				}
				return std::move(*element_type);
			}

			type operator()(call const &expr) const
			{
				type function_type = type_of_expression(expr.function);
				signature const * const sig = boost::get<signature>(&function_type);
				if (!sig)
				{
					throw std::runtime_error("The expression does not evaluate to something callable");
				}
				return sig->result;
			}

			type operator()(local_expression const &expr) const
			{
				return expr.type;
			}
		};

		inline type type_of_expression(expression const &expr)
		{
			return boost::apply_visitor(expression_type_visitor{}, expr);
		}

		inline bool is_callable(expression const &function, std::vector<expression> const &arguments)
		{
			type function_type = type_of_expression(function);
			signature const * const sig = boost::get<signature>(&function_type);
			if (!sig)
			{
				throw std::runtime_error("The expression does not evaluate to something callable");
			}
			std::vector<type> argument_types;
			std::transform(begin(arguments), end(arguments), std::back_inserter(argument_types), type_of_expression);
			return (sig->parameters == argument_types);
		}

		expression analyze(ast::expression const &syntax, name_space &names);

		value evaluate_const(expression const &expr);

		struct const_expression_evaluator : boost::static_visitor<value>
		{
			value operator()(constant_expression const &expr) const
			{
				return expr.constant;
			}

			value operator()(make_closure const &) const
			{
				throw std::runtime_error("A closure is not a compile-time value");
			}

			value operator()(subscript const &expr) const
			{
				auto left = evaluate_const(expr.left);
				auto element = boost::apply_visitor(subscription_visitor{expr.element}, left);
				if (!element)
				{
					throw std::runtime_error("Cannot get element from map at compile-time: " + expr.element);
				}
				return std::move(*element);
			}

			value operator()(call const &) const
			{
				throw std::runtime_error("Functions cannot be evaluated at compile-time");
			}

			value operator()(local_expression const &expr) const
			{
				if (!expr.const_value)
				{
					throw std::runtime_error("This definition cannot be evaluated at compile-time: " + expr.name);
				}
				return *expr.const_value;
			}
		};

		inline value evaluate_const(expression const &expr)
		{
			return boost::apply_visitor(const_expression_evaluator{}, expr);
		}

		block analyze_block(ast::block const &syntax, name_space &names);

		struct expression_analyzer : boost::static_visitor<expression>
		{
			explicit expression_analyzer(name_space &names)
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
				auto expr = require_local_identifier(m_names, syntax.position.content);
				if (!expr)
				{
					throw std::runtime_error("Unknown identifier " + syntax.position.content);
				}
				return std::move(*expr);
			}

			expression operator()(ast::lambda const &syntax) const
			{
				name_space locals{&m_names, {}, {}};
				std::vector<parameter> parameters;
				{
					std::size_t parameter_index = 0;
					for (ast::parameter const &parameter_syntax : syntax.parameters)
					{
						auto type_expr = analyze(parameter_syntax.type, locals);
						auto type = evaluate_const(type_expr);
						parameters.emplace_back(parameter{type, parameter_syntax.name.content});
						name_space_entry entry{local_identifier{local::argument, parameter_index}, type, boost::none};
						if (!locals.definitions.insert(std::make_pair(parameter_syntax.name.content, entry)).second)
						{
							throw std::runtime_error("Cannot redefine " + parameter_syntax.name.content);
						}
						++parameter_index;
					}
				}
				block body = analyze_block(syntax.body, locals);
				return make_closure{std::move(parameters), std::move(body), std::move(locals.bind_from_parent)};
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

			name_space &m_names;
		};

		inline expression analyze(ast::expression const &syntax, name_space &names)
		{
			return boost::apply_visitor(expression_analyzer{names}, syntax);
		}

		inline block analyze_block(ast::block const &syntax, name_space &locals)
		{
			block body;
			std::size_t definition_index = 0;
			for (ast::definition const &definition_syntax : syntax.elements)
			{
				auto value = analyze(definition_syntax.value, locals);
				body.definitions.emplace_back(definition{definition_syntax.name.content, value});
				name_space_entry entry{local_identifier{local::definition, definition_index}, type_of_expression(value), boost::none};
				try
				{
					entry.const_value = evaluate_const(value);
				}
				catch (std::runtime_error const &) //TODO
				{
				}
				if (!locals.definitions.insert(std::make_pair(definition_syntax.name.content, entry)).second)
				{
					throw std::runtime_error("Cannot redefine " + definition_syntax.name.content);
				}
				++definition_index;
			}
			body.result = analyze(syntax.result, locals);
			return body;
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

		inline void print(Si::sink<char> &sink, block const &b)
		{
			for (definition const &def : b.definitions)
			{
				Si::append(sink, def.name);
				Si::append(sink, " = ");
				print(sink, def.value);
				Si::append(sink, "\n");
			}
			print(sink, b.result);
		}

		inline void print(Si::sink<char> &sink, local_identifier const &id)
		{
			switch (id.type)
			{
			case local::argument: Si::append(sink, "arg:"); break;
			case local::bound: Si::append(sink, "bnd:"); break;
			case local::definition: Si::append(sink, "def:"); break;
			}
			Si::append(sink, boost::lexical_cast<std::string>(id.index));
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
				print(m_out, expr.body);
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
				Si::append(m_out, "(");
				for (auto &argument : expr.arguments)
				{
					print(m_out, argument);
					Si::append(m_out, ", ");
				}
				Si::append(m_out, ")");
			}

			void operator()(local_expression const &expr) const
			{
				Si::append(m_out, expr.name);
				Si::append(m_out, "[");
				print(m_out, expr.which);
				Si::append(m_out, "]");
				if (expr.const_value)
				{
					Si::append(m_out, "=");
					print(m_out, *expr.const_value);
				}
			}

		private:

			Si::sink<char> &m_out;
		};

		inline void print(Si::sink<char> &sink, expression const &expr)
		{
			return boost::apply_visitor(expression_printer{sink}, expr);
		}

		inline std::ostream &operator << (std::ostream &out, expression const &expr)
		{
			Si::ostream_ref_sink sink(out);
			print(sink, expr);
			return out;
		}

		inline std::ostream &operator << (std::ostream &out, block const &b)
		{
			Si::ostream_ref_sink sink(out);
			print(sink, b);
			return out;
		}
	}
}

#endif
