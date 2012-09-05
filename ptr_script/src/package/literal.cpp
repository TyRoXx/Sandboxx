#include "literal.hpp"


namespace ptrs
{
	literal::literal(
		std::unique_ptr<ptrs::type> type,
		boost::any value
		)
		: value(std::move(type))
		, m_value(std::move(value))
	{
	}

	const boost::any &literal::get() const
	{
		return m_value;
	}
}
