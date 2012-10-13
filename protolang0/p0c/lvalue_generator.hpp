#pragma once
#ifndef P0C_LVALUE_GENERATOR_HPP
#define P0C_LVALUE_GENERATOR_HPP


#include "expression_tree.hpp"
#include "reference.hpp"
#include "p0i/emitter.hpp"


namespace p0
{
	struct symbol_table;
	struct code_generator;


	struct lvalue_generator : expression_tree_visitor
	{
		explicit lvalue_generator(
			code_generator &function_generator,
			intermediate::emitter &emitter,
			symbol_table &symbols
			);
		reference address() const;

	private:

		code_generator &m_function_generator;
		intermediate::emitter &m_emitter;
		symbol_table &m_symbols;
		reference m_address;


		virtual void visit(name_expression_tree const &expression) override;
		virtual void visit(integer_10_expression_tree const &expression) override;
		virtual void visit(call_expression_tree const &expression) override;
		virtual void visit(function_tree const &expression) override;
		virtual void visit(null_expression_tree const &expression) override;
	};
}


#endif