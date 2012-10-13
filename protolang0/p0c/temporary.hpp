#pragma once
#ifndef P0C_TEMPORARY_HPP
#define P0C_TEMPORARY_HPP


#include "reference.hpp"


namespace p0
{
	struct symbol_table;


	struct temporary
	{
		explicit temporary(
			symbol_table &frame,
			size_t size
			);
		~temporary();
		reference address() const;
		size_t size() const;

	private:

		symbol_table &m_frame;
		reference const m_address;
		size_t const m_size;
	};
}


#endif
