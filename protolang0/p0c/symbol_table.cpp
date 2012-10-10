#include "symbol_table.hpp"


namespace p0
{
	symbol_table::symbol_table(
		symbol_table const *parent
		)
		: m_parent(parent)
	{
	}

	bool symbol_table::add_symbol(
		std::string name,
		symbol symbol
		)
	{
		auto const s = m_symbols_by_name.find(name);
		if (s == m_symbols_by_name.end())
		{
			m_symbols_by_name.insert(
				s,
				std::make_pair(std::move(name), symbol));
			return true;
		}

		return false;
	}

	symbol const *symbol_table::find_symbol(
		std::string const &name
		) const
	{
		auto const s = m_symbols_by_name.find(name);
		return (s == m_symbols_by_name.end()) ?
			(m_parent ? m_parent->find_symbol(name) : 0) :
			&(s->second);
	}
}
