#include "statement_tree.hpp"
#include "expression_tree.hpp"


namespace p0
{
	statement_tree_visitor::~statement_tree_visitor()
	{
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

	declaration_tree::~declaration_tree()
	{
	}

	void declaration_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	std::string const &declaration_tree::name() const
	{
		return m_name;
	}

	expression_tree const &declaration_tree::value() const
	{
		return *m_value;
	}


	return_tree::return_tree(
		std::unique_ptr<expression_tree> value
		)
		: m_value(std::move(value))
	{
	}

	return_tree::~return_tree()
	{
	}

	void return_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &return_tree::value() const
	{
		return *m_value;
	}


	block_tree::block_tree(
		statement_vector body
		)
		: m_body(std::move(body))
	{
	}

	void block_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	block_tree::statement_vector const &block_tree::body() const
	{
		return m_body;
	}


	expression_statement_tree::expression_statement_tree(
		std::unique_ptr<expression_tree> expression
		)
		: m_expression(std::move(expression))
	{
	}

	expression_statement_tree::~expression_statement_tree()
	{
	}

	void expression_statement_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &expression_statement_tree::expression() const
	{
		return *m_expression;
	}
}
