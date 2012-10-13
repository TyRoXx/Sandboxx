#pragma once
#ifndef P0C_CODE_GENERATOR_HPP
#define P0C_CODE_GENERATOR_HPP


#include "p0i/unit.hpp"
#include <functional>


namespace p0
{
	struct statement_tree;
	struct expression_tree;
	struct function_tree;
	struct symbol_table;
	struct compiler_error;

	namespace intermediate
	{
		struct emitter;
	}


	struct code_generator
	{
		typedef std::function<bool (compiler_error const &)> compiler_error_handler;


		explicit code_generator(
			function_tree const &tree,
			compiler_error_handler error_handler
			);
		intermediate::unit generate_unit();
		size_t generate_function(
			function_tree const &function
			);
		void handle_error(
			compiler_error const &error
			);

	private:

		function_tree const &m_tree;
		compiler_error_handler const m_error_handler;
		intermediate::unit::function_vector m_functions;
	};
}


#endif
