#ifndef NEW_LANG_SEMANTIC_ANALYZE_HPP
#define NEW_LANG_SEMANTIC_ANALYZE_HPP

#include "program.hpp"
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/bind.hpp>

namespace nl
{
	namespace il
	{
		struct name_space_entry
		{
			local_identifier which;
			il::type type;
			boost::optional<value> const_value;
		};

		struct name_space
		{
			name_space *next = nullptr;
			boost::unordered_map<std::string, name_space_entry> definitions;
			boost::unordered_map<local_identifier, value> constant_locals;
			std::vector<local_identifier> bind_from_parent;

			BOOST_DEFAULTED_FUNCTION(name_space(), {})

			name_space(name_space &next, boost::unordered_map<std::string, name_space_entry> definitions, std::vector<local_identifier> bind_from_parent)
				: next(&next)
				, definitions(std::move(definitions))
				, bind_from_parent(std::move(bind_from_parent))
			{
			}
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

			local type = local::constant;
			std::size_t bound_index = std::numeric_limits<std::size_t>::max();
			if (!in_parent->const_value)
			{
				type = local::bound;
				bound_index = where.bind_from_parent.size();
				where.bind_from_parent.emplace_back(in_parent->which);
			}
			return local_expression{local_identifier{type, bound_index}, in_parent->type, in_parent->name, in_parent->const_value};
		}

		type type_of_expression(expression const &expr, name_space const &environment);

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

			boost::optional<type> operator()(indirect_value const &v) const
			{
				return boost::apply_visitor(*this, *v.actual);
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

			type operator()(map const &v) const
			{
				map v_type;
				for (auto const &element : v.elements)
				{
					v_type.elements.insert(std::make_pair(element.first, type_of_value(element.second)));
				}
				return std::move(v_type);
			}

			type operator()(signature const &) const
			{
				return meta_type{};
			}

			type operator()(generic_signature const &) const
			{
				return meta_type{};
			}

			type operator()(external const &v) const
			{
				return v.type;
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

		struct result_of_call_visitor : boost::static_visitor<boost::optional<type>>
		{
			std::vector<expression> const &arguments;
			name_space const &environment;

			explicit result_of_call_visitor(std::vector<expression> const &arguments, name_space const &environment)
				: arguments(arguments)
				, environment(environment)
			{
			}

			boost::optional<type> operator()(signature const &s) const
			{
				return s.result;
			}

			boost::optional<type> operator()(generic_signature const &s) const
			{
				if (arguments.size() != s.parameters.size())
				{
					return boost::none;
				}
				for (size_t i = 0; i < arguments.size(); ++i)
				{
					if (!s.parameters[i](type_of_expression(arguments[i], environment)))
					{
						return boost::none;
					}
				}
				return s.resolve(arguments, environment);
			}

			template <class Other>
			boost::optional<type> operator()(Other const &) const
			{
				return boost::none;
			}
		};

		inline boost::optional<type> result_of_call(type const &callee, std::vector<expression> const &arguments, name_space const &environment)
		{
			return boost::apply_visitor(result_of_call_visitor{arguments, environment}, callee);
		}

		struct expression_type_visitor : boost::static_visitor<type>
		{
			name_space const &environment;

			explicit expression_type_visitor(name_space const &environment)
				: environment(environment)
			{
			}

			type operator()(constant_expression const &expr) const
			{
				return type_of_value(expr.constant);
			}

			type operator()(make_closure const &closure) const
			{
				signature sig;
				sig.result = type_of_expression(closure.body.result, environment);
				for (parameter const &param : closure.parameters)
				{
					sig.parameters.emplace_back(param.type);
				}
				return std::move(sig);
			}

			type operator()(subscript const &expr) const
			{
				auto left = type_of_expression(expr.left, environment);
				auto element_type = boost::apply_visitor(subscription_visitor{expr.element}, left);
				if (!element_type)
				{
					throw std::runtime_error("Cannot deduce type because element does not exist: " + expr.element);
				}
				return std::move(*element_type);
			}

			type operator()(call const &expr) const
			{
				type function_type = type_of_expression(expr.function, environment);
				auto result = result_of_call(function_type, expr.arguments, environment);
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

		inline type type_of_expression(expression const &expr, name_space const &environment)
		{
			return boost::apply_visitor(expression_type_visitor{environment}, expr);
		}

		struct convertible
		{
		};

		struct totally_different
		{
		};

		struct map_elements_missing
		{
			map missing_elements;
		};

		typedef boost::variant<
			convertible,
			totally_different,
			map_elements_missing
		> convertability;

		inline convertability determine_convertability(type const &from, type const &into)
		{
			if (from == into)
			{
				return convertible{};
			}
			{
				auto const * const from_map = boost::get<map>(&from);
				auto const * const into_map = boost::get<map>(&into);
				if (from_map && into_map)
				{
					map missing_elements;
					for (auto const &required_element : into_map->elements)
					{
						auto const found = from_map->elements.find(required_element.first);
						if (found != from_map->elements.end())
						{
							if (required_element.second == found->second)
							{
								continue;
							}
						}
						missing_elements.elements.insert(required_element);
					}
					if (missing_elements.elements.empty())
					{
						return convertible{};
					}
					return map_elements_missing{std::move(missing_elements)};
				}
			}
			return totally_different{};
		}

		inline bool is_convertible(type const &from, type const &into)
		{
			auto conv = determine_convertability(from, into);
			return boost::get<convertible>(&conv);
		}

		struct convertability_formatter : boost::static_visitor<boost::optional<std::string>>
		{
			boost::optional<std::string> operator()(convertible const &) const
			{
				return boost::none;
			}

			boost::optional<std::string> operator()(totally_different const &) const
			{
				return std::string("totally different");
			}

			boost::optional<std::string> operator()(map_elements_missing const &) const
			{
				return std::string("there are map elements missing");
			}
		};

		inline boost::optional<std::string> format_convertability_error(convertability const &conv)
		{
			return boost::apply_visitor(convertability_formatter{}, conv);
		}

		struct checked_callable
		{
		};

		struct wrong_argument_count
		{
			std::size_t provided, expected;
		};

		struct type_not_callable_at_all
		{
			type not_callable;
		};

		struct generic_argument_type_not_applicable
		{
			std::size_t argument_index;
			type argument_type;
		};

		struct argument_type_not_applicable
		{
			std::size_t argument_index;
			type expected;
			type provided;
		};

		typedef boost::variant<checked_callable, wrong_argument_count, type_not_callable_at_all, generic_argument_type_not_applicable, argument_type_not_applicable> callability;

		struct callability_visitor : boost::static_visitor<callability>
		{
			std::vector<type> const &argument_types;

			explicit callability_visitor(std::vector<type> const &argument_types)
				: argument_types(argument_types)
			{
			}

			callability operator()(signature const &callee) const
			{
				if (argument_types.size() != callee.parameters.size())
				{
					return wrong_argument_count{argument_types.size(), callee.parameters.size()};
				}
				for (size_t i = 0; i < argument_types.size(); ++i)
				{
					if (!is_convertible(argument_types[i], callee.parameters[i]))
					{
						return argument_type_not_applicable{i, callee.parameters[i], argument_types[i]};
					}
				}
				return checked_callable{};
			}

			callability operator()(generic_signature const &callee) const
			{
				if (argument_types.size() != callee.parameters.size())
				{
					return wrong_argument_count{argument_types.size(), callee.parameters.size()};
				}
				for (size_t i = 0; i < argument_types.size(); ++i)
				{
					if (!callee.parameters[i](argument_types[i]))
					{
						return generic_argument_type_not_applicable{i, argument_types[i]};
					}
				}
				return checked_callable{};
			}

			template <class Other>
			callability operator()(Other const &other) const
			{
				return type_not_callable_at_all{other};
			}
		};

		inline callability is_callable(expression const &function, std::vector<expression> const &arguments, name_space const &environment)
		{
			type function_type = type_of_expression(function, environment);
			std::vector<type> argument_types;
			std::transform(begin(arguments), end(arguments), std::back_inserter(argument_types), boost::bind(type_of_expression, _1, boost::ref(environment)));
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

		inline boost::optional<value> call_const_closure(value const &maybe_closure, std::vector<value> const &arguments)
		{
			return boost::apply_visitor(const_closure_caller{arguments}, maybe_closure);
		}

		expression analyze(ast::expression const &syntax, name_space &names, std::string const *defined_name);

		boost::optional<value> evaluate_const(expression const &expr, name_space const &environment);

		struct const_expression_evaluator : boost::static_visitor<boost::optional<value>>
		{
			name_space const &environment;

			explicit const_expression_evaluator(name_space const &environment)
				: environment(environment)
			{
			}

			boost::optional<value> operator()(constant_expression const &expr) const
			{
				return expr.constant;
			}

			boost::optional<value> operator()(make_closure const &closure) const
			{
				signature sig;
				sig.result = type_of_expression(closure.body.result, environment);
				for (il::parameter const &parameter : closure.parameters)
				{
					sig.parameters.emplace_back(parameter.type);
				}
				std::vector<value> bound;
				for (local_identifier const &bound_value : closure.bind_from_parent)
				{
					auto const found = environment.constant_locals.find(bound_value);
					if (found == environment.constant_locals.end())
					{
						return boost::none;
					}
					bound.emplace_back(found->second);
				}
				auto body = closure.body;
				auto call = [bound, body](std::vector<value> const &arguments) -> value
				{
					name_space locals;
					for (size_t i = 0; i < bound.size(); ++i)
					{
						locals.constant_locals.insert(std::make_pair(local_identifier{local::bound, i}, bound[i]));
					}
					for (size_t i = 0; i < arguments.size(); ++i)
					{
						locals.constant_locals.insert(std::make_pair(local_identifier{local::argument, i}, arguments[i]));
					}
					std::vector<value> definitions;
					for (il::definition const &definition : body.definitions)
					{
						auto defined_value = evaluate_const(definition.value, locals);
						assert(defined_value);
						auto const definition_index = definitions.size();
						locals.constant_locals.insert(std::make_pair(local_identifier{local::definition, definition_index}, *defined_value));
						definitions.emplace_back(*defined_value);
					}
					auto result = evaluate_const(body.result, locals);
					assert(result);
					return *result;
				};
				return value{compile_time_closure{type{sig}, call}};
			}

			boost::optional<value> operator()(subscript const &expr) const
			{
				auto left = evaluate_const(expr.left, environment);
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
				auto function = evaluate_const(expr.function, environment);
				if (!function)
				{
					return boost::none;
				}
				auto arguments = expr.arguments | boost::adaptors::transformed(boost::bind(evaluate_const, _1, boost::ref(environment)));
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
					auto const found = environment.constant_locals.find(expr.which);
					if (found != environment.constant_locals.end())
					{
						return found->second;
					}
					return boost::none;
				}
				return *expr.const_value;
			}
		};

		inline boost::optional<value> evaluate_const(expression const &expr, name_space const &environment)
		{
			return boost::apply_visitor(const_expression_evaluator{environment}, expr);
		}

		void print(Si::sink<char> &sink, value const &v);

		struct callability_error_formatter : boost::static_visitor<boost::optional<std::string>>
		{
			boost::optional<std::string> operator()(checked_callable const &) const
			{
				return boost::none;
			}

			boost::optional<std::string> operator()(wrong_argument_count const &error) const
			{
				return boost::str(boost::format("Expected %1% arguments, got %2%") % error.expected % error.provided);
			}

			boost::optional<std::string> operator()(type_not_callable_at_all const &error) const
			{
				std::string message;
				auto sink = Si::make_container_sink(message);
				Si::append(sink, "Type cannot be called at all: ");
				print(sink, error.not_callable);
				return message;
			}

			boost::optional<std::string> operator()(generic_argument_type_not_applicable const &error) const
			{
				std::string message;
				auto sink = Si::make_container_sink(message);
				Si::append(sink, "Generic argument ");
				Si::append(sink, boost::lexical_cast<std::string>(error.argument_index));
				Si::append(sink, " of type ");
				print(sink, error.argument_type);
				Si::append(sink, " is not applicable");
				return message;
			}

			boost::optional<std::string> operator()(argument_type_not_applicable const &error) const
			{
				std::string message;
				auto sink = Si::make_container_sink(message);
				Si::append(sink, "Argument ");
				Si::append(sink, boost::lexical_cast<std::string>(error.argument_index));
				Si::append(sink, " of type ");
				print(sink, error.provided);
				Si::append(sink, " is not convertible to ");
				print(sink, error.expected);
				return message;
			}
		};

		inline boost::optional<std::string> format_callability_error(callability const callability_)
		{
			return boost::apply_visitor(callability_error_formatter{}, callability_);
		}

		std::runtime_error make_semantic_error(std::string const &message, character_position where)
		{
			return std::runtime_error(message + " (" + boost::str(boost::format("%1%:%2%") % where.line % where.column) + ")");
		}

		block analyze_block(ast::block const &syntax, name_space &names);

		struct expression_analyzer : boost::static_visitor<expression>
		{
			explicit expression_analyzer(name_space &names, std::string const *defined_name)
				: m_names(names)
				, m_defined_name(defined_name)
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
				name_space locals{m_names, boost::unordered_map<std::string, name_space_entry>{}, {}};

				std::vector<type> parameter_types;
				for (ast::parameter const &parameter_syntax : syntax.parameters)
				{
					auto type_expr = analyze(parameter_syntax.type, locals, nullptr);
					auto type = evaluate_const(type_expr, m_names);
					if (!type)
					{
						throw std::runtime_error("Type of parameter is not constant: " + parameter_syntax.name.content);
					}
					parameter_types.emplace_back(std::move(*type));
				}

				boost::optional<type> explicit_return_type;
				if (m_defined_name &&
					syntax.explicit_return_type)
				{
					auto explicit_return_type_expr = analyze(*syntax.explicit_return_type, m_names, nullptr);
					explicit_return_type = evaluate_const(explicit_return_type_expr, m_names);
					if (!explicit_return_type)
					{
						throw std::runtime_error("An explicit return type has to be a constant");
					}
					signature self_signature{*explicit_return_type, parameter_types};
					locals.definitions.insert(std::make_pair(*m_defined_name, name_space_entry{local_identifier{local::this_closure, 0}, self_signature, boost::none}));
				}

				std::vector<parameter> parameters;
				{
					std::size_t parameter_index = 0;
					for (ast::parameter const &parameter_syntax : syntax.parameters)
					{
						auto const &type = parameter_types[parameter_index];
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

				if (explicit_return_type)
				{
					auto const returned_type = type_of_expression(body.result, locals);
					if (!is_convertible(returned_type, *explicit_return_type))
					{
						throw std::runtime_error("The return value type is not convertible into the explicit return type");
					}
				}

				return make_closure{std::move(parameters), std::move(body), std::move(locals.bind_from_parent)};
			}

			expression operator()(ast::subscript const &syntax) const
			{
				auto left = analyze(syntax.left, m_names, nullptr);
				return subscript{std::move(left), syntax.element.content};
			}

			expression operator()(ast::call const &syntax) const
			{
				auto function = analyze(syntax.function, m_names, nullptr);
				std::vector<expression> arguments;
				for (auto &argument : syntax.arguments)
				{
					arguments.emplace_back(analyze(argument, m_names, nullptr));
				}
				{
					auto const callability = is_callable(function, arguments, m_names);
					auto const error_message = format_callability_error(callability);
					if (error_message)
					{
						throw make_semantic_error(*error_message, syntax.argument_list);
					}
				}
				return call{std::move(function), std::move(arguments)};
			}

		private:

			name_space &m_names;
			std::string const *m_defined_name;
		};

		inline expression analyze(ast::expression const &syntax, name_space &names, std::string const *defined_name)
		{
			auto analyzed = boost::apply_visitor(expression_analyzer{names, defined_name}, syntax);
			auto constant = evaluate_const(analyzed, names);
			if (constant)
			{
				return constant_expression{*constant};
			}
			return analyzed;
		}

		inline block analyze_block(ast::block const &syntax, name_space &locals)
		{
			block body;
			std::size_t definition_index = 0;
			for (ast::definition const &definition_syntax : syntax.elements)
			{
				auto value = analyze(definition_syntax.value, locals, &definition_syntax.name.content);
				boost::optional<il::value> const_value;
				try
				{
					const_value = evaluate_const(value, locals);
				}
				catch (std::runtime_error const &) //TODO
				{
				}
				{
					auto simplified_value = (const_value ? expression{constant_expression{*const_value}} : value);
					body.definitions.emplace_back(definition{definition_syntax.name.content, std::move(simplified_value)});
				}
				name_space_entry entry{local_identifier{local::definition, definition_index}, type_of_expression(value, locals), const_value};
				if (!locals.definitions.insert(std::make_pair(definition_syntax.name.content, entry)).second)
				{
					throw std::runtime_error("Cannot redefine " + definition_syntax.name.content);
				}
				++definition_index;
			}
			body.result = analyze(syntax.result, locals, nullptr);
			return body;
		}
	}
}

#endif
