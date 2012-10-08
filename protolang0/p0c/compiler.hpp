#pragma once
#ifndef P0C_COMPILER_HPP
#define P0C_COMPILER_HPP


#include "p0i/unit.hpp"
#include "source_range.hpp"


namespace p0
{
	struct compiler
	{
		explicit compiler(
			std::string unit_name,
			source_range source
			);
		intermediate::unit compile();

	private:

		std::string m_unit_name;
		source_range m_source;
	};
}


#endif
