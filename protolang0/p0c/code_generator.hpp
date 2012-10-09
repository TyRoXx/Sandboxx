#pragma once
#ifndef P0C_CODE_GENERATOR_HPP
#define P0C_CODE_GENERATOR_HPP


#include "p0i/unit.hpp"


namespace p0
{
	struct function_tree;
	struct statement_tree;


	struct code_generator
	{
		explicit code_generator(
			function_tree const &tree
			);
		intermediate::unit generate_unit();

	private:

		function_tree const &m_tree;
		intermediate::unit::function_vector m_functions;


		void generate_function(
			function_tree const &function
			);
		void generate_statement(
			statement_tree const &statement_tree,
			intermediate::function::instruction_vector &instructions
			);
	};
}


#endif
