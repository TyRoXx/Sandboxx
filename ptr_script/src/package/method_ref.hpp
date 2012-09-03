#ifndef METHOD_REF_HPP_INCLUDED_OSTII9FO8OAV77DG
#define METHOD_REF_HPP_INCLUDED_OSTII9FO8OAV77DG


#include "structure_ref.hpp"


namespace ptrs
{
	struct method_ref
	{
		structure_ref structure;
		std::size_t method_index;
		
		
		method_ref();
		explicit method_ref(
			const structure_ref &structure,
			std::size_t method_index
			);
	};
}


#endif
