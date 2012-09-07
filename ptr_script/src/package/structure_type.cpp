#include "structure_type.hpp"
#include "type_visitor.hpp"


namespace ptrs
{
	structure_type::structure_type(const structure_ref &ref)
		: m_ref(ref)
	{

	}

	void structure_type::accept(type_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	const structure_ref &structure_type::ref() const
	{
		return m_ref;
	}
}
