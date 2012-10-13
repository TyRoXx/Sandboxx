#pragma once
#ifndef P0C_EXPRESSION_TREE_HPP
#define P0C_EXPRESSION_TREE_HPP


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
	struct null_expression_tree;


	struct expression_tree_visitor
	{
		virtual ~expression_tree_visitor();
		virtual void visit(name_expression_tree const &expression) = 0;
		virtual void visit(integer_10_expression_tree const &expression) = 0;
		virtual void visit(call_expression_tree const &expression) = 0;
		virtual void visit(function_tree const &expression) = 0;
		virtual void visit(null_expression_tree const &expression) = 0;
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
		typedef std::vector<source_range> name_vector;


		explicit function_tree(
			std::unique_ptr<statement_tree> body,
			name_vector parameters
			);
		~function_tree();
		virtual void accept(expression_tree_visitor &visitor) const override;
		statement_tree const &body() const;
		name_vector const &parameters() const;

	private:

		std::unique_ptr<statement_tree> m_body;
		name_vector m_parameters;
	};


	struct null_expression_tree : expression_tree
	{
		explicit null_expression_tree(
			source_range position
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		source_range const &position() const;

	private:

		source_range m_position;
	};
}


#endif
