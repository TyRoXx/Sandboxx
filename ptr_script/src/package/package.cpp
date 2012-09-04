#include "package.hpp"


namespace ptrs
{
	package::package(
		structure_vector structures,
		dependency_vector dependencies
		)
		: m_structures(std::move(structures))
		, m_dependencies(std::move(dependencies))
	{
	}

	const package::structure_vector &package::structures() const
	{
		return m_structures;
	}

	const package::dependency_vector &package::dependencies() const
	{
		return m_dependencies;
	}
}
