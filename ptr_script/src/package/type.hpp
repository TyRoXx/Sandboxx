#ifndef TYPE_HPP_INCLUDED_HDD3X88C4HHZ55QQ
#define TYPE_HPP_INCLUDED_HDD3X88C4HHZ55QQ


#include <ostream>


namespace ptrs
{
	struct type
	{
		explicit type();
		virtual ~type();
		virtual void print_name(std::ostream &os) const = 0;
	};
}


#endif
