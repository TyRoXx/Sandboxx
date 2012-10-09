#include "function_tree.hpp"


namespace p0
{
	expression_tree_visitor::~expression_tree_visitor()
	{
	}


	expression_tree::~expression_tree()
	{
	}


	name_expression_tree::name_expression_tree(
		source_range name
		)
		: m_name(name)
	{
	}

	source_range const &name_expression_tree::name() const
	{
		return m_name;
	}

	void name_expression_tree::accept(expression_tree_visitor &visitor)
	{
		visitor.visit(*this);
	}


	integer_10_expression_tree::integer_10_expression_tree(
		source_range value
		)
		: m_value(value)
	{
	}

	source_range const &integer_10_expression_tree::value() const
	{
		return m_value;
	}

	void integer_10_expression_tree::accept(expression_tree_visitor &visitor)
	{
		visitor.visit(*this);
	}

	
	statement_tree::~statement_tree()
	{
	}


	declaration_tree::declaration_tree(
		std::string name,
		std::unique_ptr<expression_tree> value
		)
		: m_name(std::move(name))
		, m_value(std::move(value))
	{
	}

	std::string const &declaration_tree::name() const
	{
		return m_name;
	}

	expression_tree const &declaration_tree::value() const
	{
		return *m_value;
	}


	function_tree::function_tree(
		statements body
		)
		: m_body(std::move(body))
	{
	}

	function_tree::function_tree(function_tree &&other)
		: m_body(std::move(other.m_body))
	{
	}

	function_tree &function_tree::operator = (function_tree &&other)
	{
		function_tree(std::move(other)).swap(*this);
		return *this;
	}

	void function_tree::swap(function_tree &other)
	{
		m_body.swap(other.m_body);
	}

	function_tree::statements const &function_tree::body() const
	{
		return m_body;
	}
}
