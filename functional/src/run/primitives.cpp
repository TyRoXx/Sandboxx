#include "primitives.hpp"
#include "interpreter.hpp"
#include <boost/scope_exit.hpp>


namespace fct
{
	Integer::Integer(Value value)
		: m_value(value)
	{
	}

	Integer::Value Integer::getValue() const
	{
		return m_value;
	}

	void Integer::print(std::ostream &os) const
	{
		os << m_value;
	}

	std::unique_ptr<Object> Integer::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (!arguments.empty())
		{
			throw std::runtime_error("Integer cannot be called");
		}

		return std::unique_ptr<Object>(new Integer(m_value));
	}

	bool Integer::equals(const Object &other) const
	{
		const auto * const otherInt = dynamic_cast<const Integer *>(&other);
		return otherInt &&
			(m_value == otherInt->m_value);
	}

	bool Integer::toBoolean() const
	{
		return (m_value != 0);
	}


	Boolean::Boolean(bool value)
		: m_value(value)
	{
	}

	void Boolean::print(std::ostream &os) const
	{
		os << (m_value ? true : false);
	}

	std::unique_ptr<Object> Boolean::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (!arguments.empty())
		{
			throw std::runtime_error("Boolean cannot be called");
		}

		return std::unique_ptr<Object>(new Boolean(m_value));
	}

	bool Boolean::equals(const Object &other) const
	{
		const auto * const otherBool = dynamic_cast<const Boolean *>(&other);
		return otherBool &&
			(m_value == otherBool->m_value);
	}

	bool Boolean::toBoolean() const
	{
		return m_value;
	}


	void Evaluate::print(std::ostream &os) const
	{
		os << "evaluate";
	}

	std::unique_ptr<Object> Evaluate::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (arguments.size() != 1)
		{
			throw std::runtime_error("Evaluate expects exactly one argument");
		}

		return interpreter.evaluate(arguments.front());
	}

	bool Evaluate::equals(const Object &other) const
	{
		return dynamic_cast<const Evaluate *>(&other) != 0;
	}


	void Define::print(std::ostream &os) const
	{
		os << "evaluate";
	}

	std::unique_ptr<Object> Define::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (arguments.size() != 3)
		{
			throw std::runtime_error("Define expects exactly three arguments: name value function");
		}

		{
			std::string name = arguments[0].symbol;
			if (!arguments[0].arguments.empty())
			{
				throw std::runtime_error("Defined variable " + name + " must not have arguments");
			}
			auto value = interpreter.evaluate(arguments[1]);
			interpreter.pushSymbol(std::move(name), std::move(value));
		}

		BOOST_SCOPE_EXIT((&interpreter))
		{
			interpreter.popSymbol();
		}
		BOOST_SCOPE_EXIT_END

		auto result = interpreter.evaluate(arguments[2]);

		return result;
	}

	bool Define::equals(const Object &other) const
	{
		return dynamic_cast<const Define *>(&other) != 0;
	}


	void If::print(std::ostream &os) const
	{
		os << "if";
	}

	std::unique_ptr<Object> If::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (arguments.size() != 3)
		{
			throw std::runtime_error("If arguments are: condition on_true else");
		}

		const auto condition = interpreter.evaluate(arguments[0]);
		return interpreter.evaluate(
			arguments[condition->toBoolean() ? 1 : 2]);
	}

	bool If::equals(const Object &other) const
	{
		return dynamic_cast<const If *>(&other) != 0;
	}


	void Not::print(std::ostream &os) const
	{
		os << "not";
	}

	std::unique_ptr<Object> Not::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (arguments.size() != 1)
		{
			throw std::runtime_error("Not takes exactly one argument");
		}

		const auto positive = interpreter.evaluate(arguments[0]);
		return std::unique_ptr<Object>(new Boolean(!positive->toBoolean()));
	}

	bool Not::equals(const Object &other) const
	{
		return dynamic_cast<const Not *>(&other) != 0;
	}


	void LessThan::print(std::ostream &os) const
	{
		os << "less-than";
	}

	std::unique_ptr<Object> LessThan::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (arguments.size() != 2)
		{
			throw std::runtime_error("LessThan takes exactly two arguments");
		}

		const auto left = interpreter.evaluate(arguments[0]);
		const auto right = interpreter.evaluate(arguments[1]);
		return std::unique_ptr<Object>(new Boolean(
			dynamic_cast<const Integer &>(*left).getValue() <
			dynamic_cast<const Integer &>(*right).getValue()
			));
	}

	bool LessThan::equals(const Object &other) const
	{
		return dynamic_cast<const Not *>(&other) != 0;
	}


	Function::Function(Tree value, std::vector<std::string> arguments)
		: m_value(std::move(value))
		, m_arguments(std::move(arguments))
	{
	}

	void Function::print(std::ostream &os) const
	{
		os << m_value;
	}

	std::unique_ptr<Object> Function::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (m_arguments.size() != arguments.size())
		{
			throw std::runtime_error("Function called with wrong argument count");
		}

		size_t pushed = 0;
		BOOST_SCOPE_EXIT((&pushed) (&interpreter))
		{
			while (pushed--)
			{
				interpreter.popSymbol();
			}
		}
		BOOST_SCOPE_EXIT_END

		for (size_t i = 0; i < m_arguments.size(); ++i)
		{
			interpreter.pushSymbol(m_arguments[i],
				interpreter.evaluate(arguments[i]));

			++pushed;
		}

		return interpreter.evaluate(m_value);
	}

	bool Function::equals(const Object &other) const
	{
		const auto * const otherFunc = dynamic_cast<const Function *>(&other);
		return otherFunc &&
			(m_value == otherFunc->m_value);
	}


	void MakeLambda::print(std::ostream &os) const
	{
		os << "make-lambda";
	}

	std::unique_ptr<Object> MakeLambda::evaluate(
		Interpreter &interpreter,
		const std::vector<Tree> &arguments) const
	{
		if (arguments.empty())
		{
			throw std::runtime_error("MakeLambda requires at least one argument");
		}

		std::vector<std::string> argumentNames;
		for (size_t i = 0; i < arguments.size() - 1; ++i)
		{
			const auto &arg = arguments[i];
			if (!arg.arguments.empty())
			{
				throw std::runtime_error("A lambda parameter may not have arguments");
			}

			argumentNames.push_back(arg.symbol);
		}

		return std::unique_ptr<Object>(new Function(
			arguments.back(),
			std::move(argumentNames)));
	}

	bool MakeLambda::equals(const Object &other) const
	{
		return dynamic_cast<const MakeLambda *>(&other) != 0;
	}
}
