#pragma once
#ifndef P0C_CODE_GENERATOR_HPP
#define P0C_CODE_GENERATOR_HPP


#include "p0i/unit.hpp"


namespace p0
{
	struct statement_tree;
	struct expression_tree;
	struct function_tree;

	namespace intermediate
	{
		struct emitter;
	}


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
			intermediate::emitter &emitter
			);
		void generate_expression(
			expression_tree const &expression_tree,
			intermediate::emitter &emitter
			);
	};
}


#endif
