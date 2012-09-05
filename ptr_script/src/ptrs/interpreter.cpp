#include "interpreter.hpp"


namespace ptrs
{
	interpreter::interpreter(
		package_by_id packages
		)
		: m_packages(std::move(packages))
	{
	}

	void interpreter::call(std::size_t free_method_id)
	{

	}
}
