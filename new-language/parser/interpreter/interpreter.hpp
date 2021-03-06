#ifndef NEW_LANG_INTERPRETER_HPP
#define NEW_LANG_INTERPRETER_HPP

#include "semantic/program.hpp"

namespace nl
{
	namespace interpreter
	{
		struct object;
		typedef std::shared_ptr<object const> object_ptr;

		struct object
		{
			virtual ~object()
			{
			}
			virtual object_ptr call(std::vector<object_ptr> const &arguments) const = 0;

			virtual object_ptr subscript(std::string const &) const
			{
				throw std::logic_error("This object does not support the subscript operator");
			}
		};

		typedef std::function<object_ptr (il::local_identifier)> local_context;

		struct expression
		{
			virtual ~expression()
			{
			}
			virtual object_ptr evaluate(local_context const &context) const = 0;
		};

		struct function
		{
			std::vector<std::unique_ptr<expression>> definitions;
			std::unique_ptr<expression> result;

			function()
			{
			}

			function(
				std::vector<std::unique_ptr<expression>> definitions,
				std::unique_ptr<expression> result)
				: definitions(std::move(definitions))
				, result(std::move(result))
			{
			}

			function(function &&other)
			{
				swap(other);
			}

			function &operator = (function &&other)
			{
				swap(other);
				return *this;
			}

			void swap(function &other)
			{
				definitions.swap(other.definitions);
				result.swap(other.result);
			}

			bool is_set() const
			{
				return
					(boost::algorithm::all_of(definitions, [](std::unique_ptr<expression> const &d) { return !!d; })) &&
					result;
			}
		};

		struct closure final : object, std::enable_shared_from_this<closure>
		{
			explicit closure(function const &original, std::vector<object_ptr> bound)
				: original(&original)
				, bound(std::move(bound))
			{
				assert(this->original->is_set());
				assert((boost::algorithm::all_of(this->bound, [](object_ptr const &b)
				{
					return !!b;
				})));
			}

			object_ptr call(std::vector<object_ptr> const &arguments) const SILICIUM_OVERRIDE
			{
				std::vector<object_ptr> defined;
				local_context const context = [this, &arguments, &defined](il::local_identifier id) -> object_ptr
				{
					switch (id.type)
					{
					case il::local::bound:
						{
							if (id.index >= bound.size())
							{
								throw std::logic_error("Invalid bound index access");
							}
							auto b = bound[id.index];
							assert(b);
							return b;
						}

					case il::local::argument:
						if (id.index >= arguments.size())
						{
							throw std::logic_error("Invalid argument index access");
						}
						return arguments[id.index];

					case il::local::definition:
						if (id.index >= defined.size())
						{
							throw std::logic_error("Invalid definition index access");
						}
						return defined[id.index];

					case il::local::this_closure:
						return shared_from_this();

					case il::local::constant:
						throw std::logic_error("Constants cannot be retrieved from the bound values");
					}
					return object_ptr();
				};
				for (auto const &definition : original->definitions)
				{
					assert(definition);
					defined.emplace_back(definition->evaluate(context));
				}
				return original->result->evaluate(context);
			}

		private:

			function const *original;
			std::vector<object_ptr> bound;
		};

		struct value_object : object
		{
			il::value value;

			explicit value_object(il::value value)
				: value(std::move(value))
			{
//				assert(!boost::get<il::compile_time_closure>(&this->value));
			}

			object_ptr call(std::vector<object_ptr> const &) const SILICIUM_OVERRIDE
			{
				throw std::logic_error("Cannot call this value as a function");
			}
		};

		struct make_closure : expression
		{
			explicit make_closure(function original, std::vector<il::local_identifier> bound)
				: original(std::move(original))
				, bound(std::move(bound))
			{
				assert(this->original.is_set());
			}

			virtual object_ptr evaluate(local_context const &context) const SILICIUM_OVERRIDE
			{
				std::vector<object_ptr> bound_values;
				std::transform(begin(bound), end(bound), std::back_inserter(bound_values), context);
				assert(this->original.is_set());
				return std::make_shared<closure>(std::ref(original), std::move(bound_values));
			}

		private:

			function original;
			std::vector<il::local_identifier> bound;
		};

		struct call : expression
		{
			explicit call(std::unique_ptr<expression> function, std::vector<std::unique_ptr<expression>> arguments)
				: function(std::move(function))
				, arguments(std::move(arguments))
			{
			}

			virtual object_ptr evaluate(local_context const &context) const SILICIUM_OVERRIDE
			{
				auto actual_function = function->evaluate(context);
				std::vector<object_ptr> actual_arguments;
				std::transform(begin(arguments), end(arguments), std::back_inserter(actual_arguments), std::bind(&expression::evaluate, std::placeholders::_1, std::ref(context)));
				return actual_function->call(actual_arguments);
			}

		private:

			std::unique_ptr<expression> function;
			std::vector<std::unique_ptr<expression>> arguments;
		};

		struct local_expression : expression
		{
			explicit local_expression(il::local_identifier id)
				: id(id)
			{
				assert(id.type != il::local::constant);
			}

			virtual object_ptr evaluate(local_context const &context) const SILICIUM_OVERRIDE
			{
				return context(id);
			}

		private:

			il::local_identifier id;
		};

		struct constant_expression : expression
		{
			explicit constant_expression(il::value value)
				: value(std::move(value))
			{
//				assert(!boost::get<il::compile_time_closure>(&value));
			}

			virtual object_ptr evaluate(local_context const &) const SILICIUM_OVERRIDE
			{
				return std::make_shared<value_object>(value);
			}

		private:

			il::value value;
		};

		struct subscript : expression
		{
			explicit subscript(std::unique_ptr<expression> left, std::string element)
				: left(std::move(left))
				, element(std::move(element))
			{
			}

			virtual object_ptr evaluate(local_context const &context) const SILICIUM_OVERRIDE
			{
				object_ptr const left_value = left->evaluate(context);
				return left_value->subscript(element);
			}

		private:

			std::unique_ptr<expression> left;
			std::string element;
		};
	}
}

#endif
