#include "structure_ref.hpp"


namespace ptrs
{
	structure_ref::structure_ref()
		: structure_index(std::numeric_limits<decltype(structure_index)>::max())
	{
	}

	structure_ref::structure_ref(
		package_ref package,
		std::size_t structure_index
		)
		: package(package)
		, structure_index(structure_index)
	{
	}


	bool operator == (const structure_ref &left, const structure_ref &right)
	{
		return
			(left.package == right.package) &&
			(left.structure_index == right.structure_index);
	}
}
