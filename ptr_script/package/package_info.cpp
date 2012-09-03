#include "package_info.hpp"


namespace ptrs
{
	package_info::package_info(
		const guid &id,
		std::string title,
		dependency_set dependencies
		)
		: m_id(id)
		, m_title(std::move(title))
		, m_dependencies(std::move(dependencies))
	{
	}

	const guid &package_info::id() const
	{
		return m_id;
	}

	const std::string &package_info::title() const
	{
		return m_title;
	}

	const dependency_set &package_info::dependencies() const
	{
		return m_dependencies;
	}
}
