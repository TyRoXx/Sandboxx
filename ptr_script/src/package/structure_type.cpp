#include "structure_type.hpp"


namespace ptrs
{
	structure_type::structure_type(const structure_ref &ref)
		: m_ref(ref)
	{

	}

	const structure_ref &structure_type::ref() const
	{
		return m_ref;
	}
}
