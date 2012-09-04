#ifndef POINTER_HPP_INCLUDED_VRSX7DYTR7K82YZV
#define POINTER_HPP_INCLUDED_VRSX7DYTR7K82YZV


#include "value.hpp"


namespace ptrs
{
	struct ptr_type;


	struct pointer : value
	{
		explicit pointer(std::unique_ptr<ptr_type> type);

	private:
	};
}


#endif
