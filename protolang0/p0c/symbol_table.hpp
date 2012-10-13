#pragma once
#ifndef P0C_SYMBOL_TABLE_HPP
#define P0C_SYMBOL_TABLE_HPP


#include "reference.hpp"
#include "source_range.hpp"
#include <map>
#include <string>


namespace p0
{
	struct symbol_table
	{
		explicit symbol_table(
			symbol_table const *parent
			);
		reference declare_variable(
			source_range name
			);
		reference const *find_symbol(
			std::string const &name
			) const;

	private:

		typedef std::map<std::string, reference> symbols_by_name;


		symbol_table const * const m_parent;
		symbols_by_name m_symbols_by_name;
		size_t m_next_local_address;
	};
}


#endif
