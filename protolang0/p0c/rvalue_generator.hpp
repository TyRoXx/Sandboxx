#pragma once
#ifndef P0C_RVALUE_GENERATOR_HPP
#define P0C_RVALUE_GENERATOR_HPP


#include "expression_tree.hpp"
#include "reference.hpp"
#include "p0i/emitter.hpp"
#include <functional>


namespace p0
{
	struct symbol_table;
	struct code_generator;


	struct rvalue_generator : expression_tree_visitor
	{
		///@destination may be invalid to indicate that the value is not supposed to be stored
		explicit rvalue_generator(
			code_generator &function_generator,
			intermediate::emitter &emitter,
			symbol_table &symbols,
			reference destination
			);

	private:

		code_generator &m_function_generator;
		intermediate::emitter &m_emitter;
		symbol_table &m_symbols;
		reference const m_destination;


		virtual void visit(name_expression_tree const &expression) override;
		virtual void visit(integer_10_expression_tree const &expression) override;
		virtual void visit(call_expression_tree const &expression) override;
		virtual void visit(function_tree const &expression) override;
		virtual void visit(null_expression_tree const &expression) override;
	};
}


#endif