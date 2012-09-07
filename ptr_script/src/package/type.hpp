#ifndef TYPE_HPP_INCLUDED_HDD3X88C4HHZ55QQ
#define TYPE_HPP_INCLUDED_HDD3X88C4HHZ55QQ


#include "common/override.hpp"


namespace ptrs
{
	struct type_visitor;


	struct type
	{
		virtual ~type();
		virtual void accept(type_visitor &visitor) const = 0;
	};
}


#endif
