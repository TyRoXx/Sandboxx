#pragma once
#ifndef P0C_STATEMENT_TREE_HPP
#define P0C_STATEMENT_TREE_HPP


#include "source_range.hpp"
#include <memory>
#include <string>
#include <vector>


namespace p0
{
	struct declaration_tree;
	struct return_tree;
	struct block_tree;
	struct expression_statement_tree;
	struct expression_tree;
	struct assignment_tree;


	struct statement_tree_visitor
	{
		virtual ~statement_tree_visitor();
		virtual void visit(declaration_tree const &statement) = 0;
		virtual void visit(return_tree const &statement) = 0;
		virtual void visit(block_tree const &statement) = 0;
		virtual void visit(expression_statement_tree const &statement) = 0;
		virtual void visit(assignment_tree const &expression) = 0;
	};


	struct statement_tree
	{
		virtual ~statement_tree();
		virtual void accept(statement_tree_visitor &visitor) const = 0;
	};


	struct declaration_tree : statement_tree
	{
		explicit declaration_tree(
			source_range name,
			std::unique_ptr<expression_tree> value
			);
		~declaration_tree();
		virtual void accept(statement_tree_visitor &visitor) const override;
		source_range const &name() const;
		expression_tree const &value() const;

	private:

		source_range m_name;
		std::unique_ptr<expression_tree> m_value;
	};


	struct return_tree : statement_tree
	{
		explicit return_tree(
			std::unique_ptr<expression_tree> value
			);
		~return_tree();
		virtual void accept(statement_tree_visitor &visitor) const override;
		expression_tree const &value() const;

	private:

		std::unique_ptr<expression_tree> m_value;
	};


	struct block_tree : statement_tree
	{
		typedef std::vector<std::unique_ptr<statement_tree>> statement_vector;


		explicit block_tree(
			statement_vector body
			);
		virtual void accept(statement_tree_visitor &visitor) const override;
		statement_vector const &body() const;

	private:

		statement_vector m_body;
	};


	struct expression_statement_tree : statement_tree
	{
		explicit expression_statement_tree(
			std::unique_ptr<expression_tree> expression
			);
		~expression_statement_tree();
		virtual void accept(statement_tree_visitor &visitor) const override;
		expression_tree const &expression() const;

	private:

		std::unique_ptr<expression_tree> m_expression;
	};


	struct assignment_tree : statement_tree
	{
		explicit assignment_tree(
			std::unique_ptr<expression_tree> destination,
			std::unique_ptr<expression_tree> source
			);
		virtual void accept(statement_tree_visitor &visitor) const override;
		expression_tree const &destination() const;
		expression_tree const &source() const;

	private:

		std::unique_ptr<expression_tree> m_destination;
		std::unique_ptr<expression_tree> m_source;
	};
}


#endif
