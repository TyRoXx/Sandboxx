#ifndef PACKAGE_REF_HPP_INCLUDED_KV90U7Y83USJXFL4
#define PACKAGE_REF_HPP_INCLUDED_KV90U7Y83USJXFL4


#include <cstddef>
#include <limits>


namespace ptrs
{
	struct package_ref
	{
		std::size_t dependency_index;
		
		
		static const std::size_t self_index = -1;
		
		
		package_ref();
		explicit package_ref(std::size_t dependency_index);
		bool is_self() const;
	};
}


#endif
