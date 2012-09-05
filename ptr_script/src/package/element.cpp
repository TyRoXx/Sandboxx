#include "element.hpp"


namespace ptrs
{
	element::element(
		std::unique_ptr<ptrs::type> type,
		std::string name
		)
		: m_type(std::move(type))
		, m_name(std::move(name))
	{
	}

	const type &element::type() const
	{
		return *m_type;
	}

	const std::string &element::name() const
	{
		return m_name;
	}
}
