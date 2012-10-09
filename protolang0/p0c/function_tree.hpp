#pragma once
#ifndef P0C_FUNCTION_TREE_HPP
#define P0C_FUNCTION_TREE_HPP


#include "source_range.hpp"
#include <memory>
#include <string>
#include <vector>


namespace p0
{
	struct name_expression_tree;
	struct integer_10_expression_tree;
	struct call_expression_tree;
	struct function_tree;


	struct expression_tree_visitor
	{
		virtual ~expression_tree_visitor();
		virtual void visit(name_expression_tree const &expression) = 0;
		virtual void visit(integer_10_expression_tree const &expression) = 0;
		virtual void visit(call_expression_tree const &expression) = 0;
		virtual void visit(function_tree const &expression) = 0;
	};


	struct expression_tree
	{
		virtual ~expression_tree();
		virtual void accept(expression_tree_visitor &visitor) const = 0;
	};


	struct name_expression_tree : expression_tree
	{
		explicit name_expression_tree(
			source_range name
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		source_range const &name() const;

	private:

		source_range m_name;
	};


	struct integer_10_expression_tree : expression_tree
	{
		explicit integer_10_expression_tree(
			source_range value
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		source_range const &value() const;

	private:

		source_range m_value;
	};


	struct call_expression_tree : expression_tree
	{
		typedef std::vector<std::unique_ptr<expression_tree>> expression_vector;


		explicit call_expression_tree(
			std::unique_ptr<expression_tree> function,
			expression_vector arguments
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		expression_tree const &function() const;
		expression_vector const &arguments() const;

	private:

		std::unique_ptr<expression_tree> m_function;
		expression_vector m_arguments;
	};


	struct statement_tree;


	struct function_tree : expression_tree
	{
		explicit function_tree(
			std::unique_ptr<statement_tree> body
			);
		function_tree(function_tree &&other);
		function_tree &operator = (function_tree &&other);
		void swap(function_tree &other);
		virtual void accept(expression_tree_visitor &visitor) const override;
		statement_tree const &body() const;

	private:

		std::unique_ptr<statement_tree> m_body;
	};


	struct declaration_tree;
	struct return_tree;
	struct block_tree;


	struct statement_tree_visitor
	{
		virtual ~statement_tree_visitor();
		virtual void visit(declaration_tree const &statement) const = 0;
		virtual void visit(return_tree const &statement) const = 0;
		virtual void visit(block_tree const &statement) const = 0;
	};


	struct statement_tree
	{
		virtual ~statement_tree();
		virtual void accept(statement_tree_visitor &visitor) const = 0;
	};


	struct declaration_tree : statement_tree
	{
		explicit declaration_tree(
			std::string name,
			std::unique_ptr<expression_tree> value
			);
		virtual void accept(statement_tree_visitor &visitor) const override;
		std::string const &name() const;
		expression_tree const &value() const;

	private:

		std::string m_name;
		std::unique_ptr<expression_tree> m_value;
	};


	struct return_tree : statement_tree
	{
		explicit return_tree(
			std::unique_ptr<expression_tree> value
			);
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
}


#endif
