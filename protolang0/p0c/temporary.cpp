#include "temporary.hpp"
#include "symbol_table.hpp"


namespace p0
{
	temporary::temporary(
		symbol_table &frame,
		size_t size
		)
		: m_frame(frame)
		, m_address(frame.allocate(size))
		, m_size(size)
	{
	}

	temporary::~temporary()
	{
		m_frame.deallocate_top(m_size);
	}

	reference temporary::address() const
	{
		return m_address;
	}

	size_t temporary::size() const
	{
		return m_size;
	}
}
