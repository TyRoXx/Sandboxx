#ifndef VALUE_HPP_INCLUDED_YJZRTXMC635LUY4Q
#define VALUE_HPP_INCLUDED_YJZRTXMC635LUY4Q


#include "common/override.hpp"


namespace ptrs
{
	struct value_visitor;


	struct value
	{
		virtual ~value();
		virtual void accept(value_visitor &visitor) const = 0;
	};
}


#endif
