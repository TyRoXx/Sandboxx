#pragma once
#ifndef P0C_EXPRESSION_CODE_GENERATOR_HPP
#define P0C_EXPRESSION_CODE_GENERATOR_HPP


#include "expression_tree.hpp"
#include "reference.hpp"
#include "p0i/emitter.hpp"


namespace p0
{
	struct symbol_table;
	struct code_generator;


	struct expression_code_generator : expression_tree_visitor
	{
		explicit expression_code_generator(
			code_generator &function_generator,
			intermediate::emitter &emitter,
			reference destination,
			symbol_table &symbols
			);
		virtual void visit(name_expression_tree const &expression) override;
		virtual void visit(integer_10_expression_tree const &expression) override;
		virtual void visit(call_expression_tree const &expression) override;
		virtual void visit(function_tree const &expression) override;
		virtual void visit(null_expression_tree const &expression) override;

	private:

		code_generator &m_function_generator;
		intermediate::emitter &m_emitter;
		reference m_destination;
		symbol_table &m_symbols;
	};


	void generate_expression(
		expression_tree const &tree,
		code_generator &function_generator,
		intermediate::emitter &emitter,
		reference destination,
		symbol_table &symbols
		);
}


#endif
