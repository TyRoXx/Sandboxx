#include "interpreter.hpp"
#include "primitives.hpp"
#include <algorithm>
#include <sstream>


namespace fct
{
	std::unique_ptr<Object> Interpreter::evaluate(const Tree &program)
	{
		const auto &symbol = program.symbol;
		const Object * const function = findSymbol(symbol);
		if (!function)
		{
			std::istringstream sstr(symbol);
			Integer::Value value;
			sstr >> value;
			if (sstr)
			{
				return std::unique_ptr<Object>(new Integer(value));
			}

			throw std::runtime_error("Unknown symbol " + symbol);
		}

		return function->evaluate(*this, program.arguments);
	}

	void Interpreter::pushSymbol(std::string name, std::unique_ptr<Object> value)
	{
		m_symbols.push_back(std::make_pair(std::move(name), std::move(value)));

		if (m_symbols.size() > 33)
		{
			throw std::runtime_error("Suspicious symbol count");
		}
	}

	void Interpreter::popSymbol()
	{
		m_symbols.pop_back();
	}

	Object *Interpreter::findSymbol(const std::string &name) const
	{
		const auto i = std::find_if(m_symbols.rbegin(), m_symbols.rend(),
			[&name](const std::pair<std::string, std::unique_ptr<Object>> &element)
		{
			return element.first == name;
		});

		return (i == m_symbols.rend() ? 0 : i->second.get());
	}

	size_t Interpreter::getSymbolCount() const
	{
		return m_symbols.size();
	}
}
