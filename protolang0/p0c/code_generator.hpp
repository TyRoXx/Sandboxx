#pragma once
#ifndef P0C_CODE_GENERATOR_HPP
#define P0C_CODE_GENERATOR_HPP


#include "p0i/unit.hpp"


namespace p0
{
	struct unit_tree;


	struct code_generator
	{
		explicit code_generator(
			unit_tree const &tree
			);
		intermediate::unit generate_unit();

	private:

		unit_tree const &m_tree;
	};
}


#endif
