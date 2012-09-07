#include "ptr_type.hpp"
#include "type_visitor.hpp"
#include <cassert>


namespace ptrs
{
	ptr_type::ptr_type(std::unique_ptr<type> pointee)
		: m_pointee(std::move(pointee))
	{
		assert(m_pointee);
	}

	void ptr_type::accept(type_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	const type &ptr_type::pointee() const
	{
		assert(m_pointee);
		return *m_pointee;
	}
}
