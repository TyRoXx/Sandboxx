#ifndef NEW_LANG_ANALYZE_HPP
#define NEW_LANG_ANALYZE_HPP

#include "ast.hpp"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>

namespace nl
{
	namespace il
	{
		struct map;
		struct signature;
		struct generic_signature;

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

#define NL_DEFINE_TRIVIAL_INLINE_STRUCT(name_) \
		struct name_ \
		{ \
		}; \
		inline bool operator == (name_ const &, name_ const &) \
		{ \
			return true; \
		} \
		inline std::size_t hash_value(name_ const &) \
		{ \
			return boost::hash_value(true); \
		}

		NL_DEFINE_TRIVIAL_INLINE_STRUCT(null)
		NL_DEFINE_TRIVIAL_INLINE_STRUCT(meta_type)
		NL_DEFINE_TRIVIAL_INLINE_STRUCT(string_type)
		NL_DEFINE_TRIVIAL_INLINE_STRUCT(integer_type)
		NL_DEFINE_TRIVIAL_INLINE_STRUCT(signature_type)

#undef NL_DEFINE_TRIVIAL_INLINE_STRUCT

		struct compile_time_closure;
		struct indirect_value;

		typedef boost::variant<
			null,
			boost::recursive_wrapper<map>,
			boost::recursive_wrapper<signature>,
			boost::recursive_wrapper<generic_signature>,
			external,
			integer,
			string,
			meta_type,
			string_type,
			integer_type,
			signature_type,
			boost::recursive_wrapper<compile_time_closure>,
			boost::recursive_wrapper<indirect_value>
		> value;

		typedef value type;

		struct indirect_value
		{
			value const *actual;
		};

		inline bool operator == (indirect_value const &left, indirect_value const &right)
		{
			if (left.actual && right.actual)
			{
				return (*left.actual == *right.actual);
			}
			return (left.actual == right.actual);
		}

		inline std::size_t hash_value(indirect_value const &value)
		{
			std::size_t digest = 0;
			if (value.actual)
			{
				using boost::hash_value;
				return hash_value(*value.actual);
			}
			return digest;
		}

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

		struct generic_signature
		{
			typedef std::function<bool (type const &)> type_predicate;

			type result;
			std::vector<type_predicate> parameters;
		};

		inline bool operator == (generic_signature const &left, generic_signature const &right)
		{
			return
					(left.result == right.result) &&
					(left.parameters.size() == right.parameters.size()); //TODO cannot really be compared
		}

		inline std::size_t hash_value(generic_signature const &value)
		{
			std::size_t digest = 0;
			boost::hash_combine(digest, value.result);
			boost::hash_combine(digest, value.parameters.size()); //TODO does not really have a hash
			return digest;
		}

		struct compile_time_closure
		{
			il::type type;
			std::function<value (std::vector<value> const &)> call;
		};

		inline bool operator == (compile_time_closure const &, compile_time_closure const &)
		{
			return true;
		}

		inline std::size_t hash_value(compile_time_closure const &)
		{
			return 0;
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

		type type_of_value(value const &v);

		struct value_type_getter : boost::static_visitor<type>
		{
			type operator()(null const &) const
			{
				return null{};
			}

			type operator()(map const &) const
			{
				throw std::logic_error("not implemented");
			}

			type operator()(signature const &) const
			{
				return meta_type{};
			}

			type operator()(generic_signature const &) const
			{
				return meta_type{};
			}

			type operator()(external const &) const
			{
				throw std::logic_error("not implemented");
			}

			type operator()(integer const &) const
			{
				return integer_type{};
			}

			type operator()(string const &) const
			{
				return string_type{};
			}

			type operator()(meta_type const &) const
			{
				return null{};
			}

			type operator()(string_type const &) const
			{
				return meta_type{};
			}

			type operator()(integer_type const &) const
			{
				return meta_type{};
			}

			type operator()(signature_type const &) const
			{
				return meta_type{};
			}

			type operator()(compile_time_closure const &v) const
			{
				return v.type;
			}

			type operator()(indirect_value const &v) const
			{
				assert(v.actual);
				return type_of_value(*v.actual);
			}
		};

		inline type type_of_value(value const &v)
		{
			return boost::apply_visitor(value_type_getter{}, v);
		}

		struct type_of_call_visitor : boost::static_visitor<boost::optional<type>>
		{
			boost::optional<type> operator()(signature const &s) const
			{
				return s.result;
			}

			boost::optional<type> operator()(generic_signature const &s) const
			{
				return s.result;
			}

			template <class Other>
			boost::optional<type> operator()(Other const &) const
			{
				return boost::none;
			}
		};

		inline boost::optional<type> result_of_call(type const &callee)
		{
			return boost::apply_visitor(type_of_call_visitor{}, callee);
		}

		struct expression_type_visitor : boost::static_visitor<type>
		{
			type operator()(constant_expression const &expr) const
			{
				return type_of_value(expr.constant);
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
				auto result = result_of_call(function_type);
				if (!result)
				{
					throw std::runtime_error("Value cannot be called as a function");
				}
				return std::move(*result);
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

		struct callability_visitor : boost::static_visitor<bool>
		{
			std::vector<type> const &argument_types;

			explicit callability_visitor(std::vector<type> const &argument_types)
				: argument_types(argument_types)
			{
			}

			bool operator()(signature const &callee) const
			{
				return (argument_types == callee.parameters);
			}

			bool operator()(generic_signature const &callee) const
			{
				if (argument_types.size() != callee.parameters.size())
				{
					return false;
				}
				for (size_t i = 0; i < argument_types.size(); ++i)
				{
					if (!callee.parameters[i](argument_types[i]))
					{
						return false;
					}
				}
				return true;
			}

			template <class Other>
			bool operator()(Other const &) const
			{
				return false;
			}
		};

		inline bool is_callable(expression const &function, std::vector<expression> const &arguments)
		{
			type function_type = type_of_expression(function);
			std::vector<type> argument_types;
			std::transform(begin(arguments), end(arguments), std::back_inserter(argument_types), type_of_expression);
			return boost::apply_visitor(callability_visitor{argument_types}, function_type);
		}

		struct const_closure_caller : boost::static_visitor<boost::optional<value>>
		{
			std::vector<value> const &arguments;

			explicit const_closure_caller(std::vector<value> const &arguments)
				: arguments(arguments)
			{
			}

			boost::optional<value> operator()(compile_time_closure const &v) const
			{
				return v.call(arguments);
			}

			template <class Other>
			boost::optional<value> operator()(Other const &) const
			{
				return boost::none;
			}
		};

		boost::optional<value> call_const_closure(value const &maybe_closure, std::vector<value> const &arguments)
		{
			return boost::apply_visitor(const_closure_caller{arguments}, maybe_closure);
		}

		expression analyze(ast::expression const &syntax, name_space &names);

		boost::optional<value> evaluate_const(expression const &expr);

		struct const_expression_evaluator : boost::static_visitor<boost::optional<value>>
		{
			boost::optional<value> operator()(constant_expression const &expr) const
			{
				return expr.constant;
			}

			boost::optional<value> operator()(make_closure const &) const
			{
				return boost::none;
			}

			boost::optional<value> operator()(subscript const &expr) const
			{
				auto left = evaluate_const(expr.left);
				if (!left)
				{
					return boost::none;
				}
				auto element = boost::apply_visitor(subscription_visitor{expr.element}, *left);
				if (!element)
				{
					return boost::none;
				}
				return std::move(*element);
			}

			boost::optional<value> operator()(call const &expr) const
			{
				auto function = evaluate_const(expr.function);
				if (!function)
				{
					return boost::none;
				}
				auto arguments = expr.arguments | boost::adaptors::transformed(evaluate_const);
				if (!boost::algorithm::all_of(arguments, [](boost::optional<value> const &v) -> bool
				{
					return v.is_initialized();
				}))
				{
					return boost::none;
				}
				std::vector<value> actual_arguments;
				boost::range::transform(arguments, std::back_inserter(actual_arguments), [](boost::optional<value> const &v)
				{
					return *v;
				});
				return call_const_closure(*function, actual_arguments);
			}

			boost::optional<value> operator()(local_expression const &expr) const
			{
				if (!expr.const_value)
				{
					return boost::none;
				}
				return *expr.const_value;
			}
		};

		inline boost::optional<value> evaluate_const(expression const &expr)
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
						if (!type)
						{
							throw std::runtime_error("Type of parameter is not constant: " + parameter_syntax.name.content);
						}
						parameters.emplace_back(parameter{*type, parameter_syntax.name.content});
						name_space_entry entry{local_identifier{local::argument, parameter_index}, *type, boost::none};
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
				boost::optional<il::value> const_value;
				try
				{
					const_value = evaluate_const(value);
				}
				catch (std::runtime_error const &) //TODO
				{
				}
				{
					auto simplified_value = (const_value ? expression{constant_expression{*const_value}} : value);
					body.definitions.emplace_back(definition{definition_syntax.name.content, std::move(simplified_value)});
				}
				name_space_entry entry{local_identifier{local::definition, definition_index}, type_of_expression(value), const_value};
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

			void operator()(generic_signature const &value) const
			{
				Si::append(m_out, "-generic_signature-(");
				Si::append(m_out, boost::lexical_cast<std::string>(value.parameters.size()));
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

			void operator()(meta_type const &) const
			{
				Si::append(m_out, "-type-");
			}

			void operator()(string_type const &) const
			{
				Si::append(m_out, "-string-");
			}

			void operator()(integer_type const &) const
			{
				Si::append(m_out, "-integer-");
			}

			void operator()(signature_type const &) const
			{
				Si::append(m_out, "-signature-");
			}

			void operator()(compile_time_closure const &) const
			{
				Si::append(m_out, "-ctclosure-");
			}

			void operator()(indirect_value const &v) const
			{
				Si::append(m_out, "-indirect-(");
				if (v.actual)
				{
					print(m_out, *v.actual);
				}
				Si::append(m_out, ")");
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
