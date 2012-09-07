#include "parameter.hpp"


namespace ptrs
{
	parameter::parameter(
		std::unique_ptr<ptrs::type> type,
		std::string name
		)
		: m_type(std::move(type))
		, m_name(std::move(name))
	{
	}

	const type &parameter::type() const
	{
		return *m_type;
	}

	const std::string &parameter::name() const
	{
		return m_name;
	}
}
