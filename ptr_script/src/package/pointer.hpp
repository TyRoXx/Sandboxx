#ifndef POINTER_HPP_INCLUDED_VRSX7DYTR7K82YZV
#define POINTER_HPP_INCLUDED_VRSX7DYTR7K82YZV


#include "value.hpp"


namespace ptrs
{
	struct pointer_type;


	struct pointer : value
	{
		explicit pointer(const pointer_type &type);

	private:
	};
}


#endif
