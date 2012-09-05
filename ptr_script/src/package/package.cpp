#include "package.hpp"


namespace ptrs
{
	package::package(
		structure_vector structures,
		method_vector free_methods,
		dependency_vector dependencies
		)
		: m_structures(std::move(structures))
		, m_free_methods(std::move(free_methods))
		, m_dependencies(std::move(dependencies))
	{
	}

	const package::structure_vector &package::structures() const
	{
		return m_structures;
	}

	const package::method_vector &package::free_methods() const
	{
		return m_free_methods;
	}

	const package::dependency_vector &package::dependencies() const
	{
		return m_dependencies;
	}
}
