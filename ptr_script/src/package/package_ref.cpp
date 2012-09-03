#include "package_ref.hpp"


namespace ptrs
{
	package_ref::package_ref()
		: dependency_index(self_index)
	{

	}

	package_ref::package_ref(std::size_t dependency_index)
		: dependency_index(dependency_index)
	{

	}

	bool package_ref::is_self() const
	{
		return (dependency_index == self_index);
	}


	bool operator == (const package_ref &left, const package_ref &right)
	{
		return (left.dependency_index == right.dependency_index);
	}
}
