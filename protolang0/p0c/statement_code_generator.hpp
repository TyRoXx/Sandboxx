#pragma once
#ifndef P0C_STATEMENT_CODE_GENERATOR_HPP
#define P0C_STATEMENT_CODE_GENERATOR_HPP


#include "statement_tree.hpp"
#include "p0i/emitter.hpp"


namespace p0
{
	struct symbol_table;


	struct statement_code_generator : statement_tree_visitor
	{
		explicit statement_code_generator(
			intermediate::emitter &emitter,
			symbol_table &symbols
			);
		virtual void visit(declaration_tree const &statement) override;
		virtual void visit(return_tree const &statement) override;
		virtual void visit(block_tree const &statement) override;
		virtual void visit(expression_statement_tree const &statement) override;
		virtual void visit(assignment_tree const &statement) override;

	private:

		intermediate::emitter &m_emitter;
		symbol_table &m_symbols;
	};


	void generate_statement(
		statement_tree const &tree,
		intermediate::emitter &emitter,
		symbol_table &symbols
		);
}


#endif
