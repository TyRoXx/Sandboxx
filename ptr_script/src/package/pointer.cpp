#include "pointer.hpp"
#include "ptr_type.hpp"


namespace ptrs
{
	pointer::pointer(std::unique_ptr<ptr_type> type)
		: value(std::move(type))
	{
	}
}
