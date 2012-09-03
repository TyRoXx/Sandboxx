#ifndef STRUCTURE_REF_HPP_INCLUDED_BOJM1V8WR3KSB56O
#define STRUCTURE_REF_HPP_INCLUDED_BOJM1V8WR3KSB56O


#include "package_ref.hpp"


namespace ptrs
{
	struct structure_ref
	{
		package_ref package;
		std::size_t structure_index;
		
		
		structure_ref();
		explicit structure_ref(
			package_ref package,
			std::size_t structure_index
			);
	};


	bool operator == (const structure_ref &left, const structure_ref &right);
}


#endif
