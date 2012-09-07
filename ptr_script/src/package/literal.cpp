#include "literal.hpp"


namespace ptrs
{
	literal::literal(
		boost::any value
		)
		: m_value(std::move(value))
	{
	}

	const boost::any &literal::get() const
	{
		return m_value;
	}
}
