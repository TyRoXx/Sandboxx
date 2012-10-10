#include "reference.hpp"


namespace p0
{
	reference::reference()
		: m_local_address(-1)
	{
	}

	reference::reference(
		size_t local_address
		)
		: m_local_address(local_address)
	{
	}

	size_t reference::local_address() const
	{
		return m_local_address;
	}

	bool reference::is_valid() const
	{
		return (m_local_address != reference().local_address());
	}
}
