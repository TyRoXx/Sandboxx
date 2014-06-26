#ifndef NEW_LANG_INTERPRETER_PREPARE_HPP
#define NEW_LANG_INTERPRETER_PREPARE_HPP

#include "interpreter/interpreter.hpp"

namespace nl
{
	namespace interpreter
	{
		template <class T, class ...Args>
		std::unique_ptr<T> make_unique(Args &&...args)
		{
			return std::unique_ptr<T>(new T{std::forward<Args>(args)...});
		}

		std::unique_ptr<expression> prepare_expression(nl::il::expression const &original);
		function prepare_block(nl::il::block const &program);

		struct expression_preparator : boost::static_visitor<std::shared_ptr<std::unique_ptr<expression>>>
		{
			std::shared_ptr<std::unique_ptr<expression>> operator()(nl::il::constant_expression const &expr) const
			{
				return std::make_shared<std::unique_ptr<expression>>(interpreter::make_unique<constant_expression>(expr.constant));
			}

			std::shared_ptr<std::unique_ptr<expression>> operator()(nl::il::make_closure const &expr) const
			{
				auto original = prepare_block(expr.body);
				return std::make_shared<std::unique_ptr<expression>>(interpreter::make_unique<make_closure>(std::move(original), expr.bind_from_parent));
			}

			std::shared_ptr<std::unique_ptr<expression>> operator()(nl::il::subscript const &expr) const
			{
				auto left = prepare_expression(expr.left);
				return std::make_shared<std::unique_ptr<expression>>(interpreter::make_unique<subscript>(std::move(left), expr.element));
			}

			std::shared_ptr<std::unique_ptr<expression>> operator()(nl::il::call const &expr) const
			{
				auto function = prepare_expression(expr.function);
				std::vector<std::unique_ptr<expression>> arguments;
				std::transform(begin(expr.arguments), end(expr.arguments), std::back_inserter(arguments), prepare_expression);
				return std::make_shared<std::unique_ptr<expression>>(interpreter::make_unique<call>(std::move(function), std::move(arguments)));
			}

			std::shared_ptr<std::unique_ptr<expression>> operator()(nl::il::local_expression const &expr) const
			{
				if (expr.which.type == nl::il::local::constant)
				{
					assert(expr.const_value);
					if (boost::get<il::compile_time_closure>(&*expr.const_value))
					{
						throw std::invalid_argument("A compile_time_closure is not a runtime expression");
					}
					return std::make_shared<std::unique_ptr<expression>>(make_unique<constant_expression>(*expr.const_value));
				}
				return std::make_shared<std::unique_ptr<expression>>(make_unique<local_expression>(expr.which));
			}
		};

		inline std::unique_ptr<expression> prepare_expression(nl::il::expression const &original)
		{
			return std::move(*boost::apply_visitor(expression_preparator{}, original));
		}

		inline function prepare_block(nl::il::block const &program)
		{
			std::vector<std::unique_ptr<expression>> definitions;
			for (nl::il::definition const &definition : program.definitions)
			{
				definitions.emplace_back(prepare_expression(definition.value));
				assert(definitions.back());
			}
			auto result = prepare_expression(program.result);
			assert(result);
			return function{std::move(definitions), std::move(result)};
		}
	}
}

#endif
