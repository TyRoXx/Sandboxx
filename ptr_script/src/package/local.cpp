#include "local.hpp"
#include "value_visitor.hpp"


namespace ptrs
{
	local::local(
		std::size_t id
		)
		: m_id(id)
	{
	}

	void local::accept(value_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	std::size_t local::id() const
	{
		return m_id;
	}
}
