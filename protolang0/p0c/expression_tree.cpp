#include "expression_tree.hpp"
#include "statement_tree.hpp"


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

	void name_expression_tree::accept(expression_tree_visitor &visitor) const
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

	void integer_10_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}


	call_expression_tree::call_expression_tree(
		std::unique_ptr<expression_tree> function,
		expression_vector arguments
		)
		: m_function(std::move(function))
		, m_arguments(std::move(arguments))
	{
	}

	void call_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &call_expression_tree::function() const
	{
		return *m_function;
	}

	call_expression_tree::expression_vector const &call_expression_tree::arguments() const
	{
		return m_arguments;
	}


	function_tree::function_tree(
		std::unique_ptr<statement_tree> body,
		name_vector parameters
		)
		: m_body(std::move(body))
		, m_parameters(std::move(parameters))
	{
	}

	function_tree::~function_tree()
	{
	}

	void function_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	statement_tree const &function_tree::body() const
	{
		return *m_body;
	}

	function_tree::name_vector const &function_tree::parameters() const
	{
		return m_parameters;
	}


	void null_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}
}
