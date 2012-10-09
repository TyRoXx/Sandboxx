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


	struct expression_tree_visitor
	{
		virtual ~expression_tree_visitor();
		virtual void visit(name_expression_tree &expression) = 0;
		virtual void visit(integer_10_expression_tree &expression) = 0;
	};


	struct expression_tree
	{
		virtual ~expression_tree();
		virtual void accept(expression_tree_visitor &visitor) = 0;
	};


	struct name_expression_tree : expression_tree
	{
		explicit name_expression_tree(
			source_range name
			);
		virtual void accept(expression_tree_visitor &visitor) override;
		source_range const &name() const;

	private:

		source_range m_name;
	};


	struct integer_10_expression_tree : expression_tree
	{
		explicit integer_10_expression_tree(
			source_range value
			);
		virtual void accept(expression_tree_visitor &visitor) override;
		source_range const &value() const;

	private:

		source_range m_value;
	};


	struct statement_tree
	{
		virtual ~statement_tree();
	};


	struct declaration_tree : statement_tree
	{
		explicit declaration_tree(
			std::string name,
			std::unique_ptr<expression_tree> value
			);
		std::string const &name() const;
		expression_tree const &value() const;

	private:

		std::string m_name;
		std::unique_ptr<expression_tree> m_value;
	};


	struct function_tree
	{
		typedef std::vector<std::unique_ptr<statement_tree>> statements;


		explicit function_tree(
			statements body
			);
		function_tree(function_tree &&other);
		function_tree &operator = (function_tree &&other);
		void swap(function_tree &other);
		statements const &body() const;

	private:

		statements m_body;
	};
}


#endif
