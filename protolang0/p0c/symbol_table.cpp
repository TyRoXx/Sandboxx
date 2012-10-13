#include "symbol_table.hpp"
#include "compiler_error.hpp"


namespace p0
{
	symbol_table::symbol_table(
		symbol_table const *parent
		)
		: m_parent(parent)
		, m_next_local_address(parent ? parent->m_next_local_address : 0)
	{
	}

	reference symbol_table::declare_variable(
		source_range name
		)
	{
		//non-const for moving later
		std::string name_str = source_range_to_string(name);

		auto const s = m_symbols_by_name.find(name_str);
		if (s != m_symbols_by_name.end())
		{
			throw compiler_error(
				"Name of local variable is already in use",
				name
				);
		}

		reference const variable_address(m_next_local_address);

		m_symbols_by_name.insert(
			s,
			std::make_pair(std::move(name_str), variable_address)
			);

		++m_next_local_address;
		return variable_address;
	}

	reference const *symbol_table::find_symbol(
		std::string const &name
		) const
	{
		auto const s = m_symbols_by_name.find(name);
		return (s == m_symbols_by_name.end()) ?
			(m_parent ? m_parent->find_symbol(name) : 0) :
			&(s->second);
	}
}
