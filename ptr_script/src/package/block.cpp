#include "block.hpp"
#include "statement_visitor.hpp"


namespace ptrs
{
	block::block(
		statement_vector statements,
		bool is_jump_target
		)
		: m_statements(std::move(statements))
		, m_is_jump_target(is_jump_target)
	{
	}

	void block::accept(statement_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	const block::statement_vector &block::statements() const
	{
		return m_statements;
	}

	bool block::is_jump_target() const
	{
		return m_is_jump_target;
	}
}
