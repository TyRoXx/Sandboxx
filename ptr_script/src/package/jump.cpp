#include "jump.hpp"
#include "statement_visitor.hpp"


namespace ptrs
{
	jump::jump(
		mode_t mode,
		std::size_t block_count
		)
		: m_mode(mode)
		, m_block_count(block_count)
	{
	}

	void jump::accept(statement_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	jump::mode_t jump::mode() const
	{
		return m_mode;
	}

	std::size_t jump::block_count() const
	{
		return m_block_count;
	}
}
