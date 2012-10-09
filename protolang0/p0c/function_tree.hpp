#pragma once
#ifndef P0C_FUNCTION_TREE_HPP
#define P0C_FUNCTION_TREE_HPP


#include "source_range.hpp"
#include <memory>
#include <string>
#include <vector>


namespace p0
{
	struct expression_tree
	{
		virtual ~expression_tree();
	};


	struct name_expression_tree : expression_tree
	{
		explicit name_expression_tree(
			source_range name
			);
		source_range const &name() const;

	private:

		source_range m_name;
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
