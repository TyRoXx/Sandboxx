#include "interpreter.hpp"


namespace ptrs
{
	interpreter::interpreter(
		package_by_id packages
		)
		: m_packages(std::move(packages))
	{
	}

	const interpreter::package_by_id &interpreter::packages() const
	{
		return m_packages;
	}
}
