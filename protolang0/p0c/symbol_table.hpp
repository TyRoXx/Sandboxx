#pragma once
#ifndef P0C_SYMBOL_TABLE_HPP
#define P0C_SYMBOL_TABLE_HPP


#include "symbol.hpp"
#include <map>
#include <string>


namespace p0
{
	struct symbol_table
	{
		explicit symbol_table(
			symbol_table const *parent
			);
		bool add_symbol(
			std::string name,
			symbol symbol
			);
		symbol const *find_symbol(
			std::string const &name
			) const;

	private:

		typedef std::map<std::string, symbol> symbols_by_name;


		symbol_table const * const m_parent;
		symbols_by_name m_symbols_by_name;
	};
}


#endif
