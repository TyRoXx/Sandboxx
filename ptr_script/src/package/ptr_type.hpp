#ifndef PTR_TYPE_HPP_INCLUDED_HVHUZUHQZSB77YU3
#define PTR_TYPE_HPP_INCLUDED_HVHUZUHQZSB77YU3


#include "type.hpp"
#include <memory>


namespace ptrs
{
	struct ptr_type : type
	{
		explicit ptr_type(std::unique_ptr<type> pointee);
		virtual void printName(std::ostream &os) const;
		
	private:
	
		std::unique_ptr<type> m_pointee;
	};
}


#endif
