#ifndef STRUCTURE_TYPE_HPP_INCLUDED_3TA98U6VI7TKF22C
#define STRUCTURE_TYPE_HPP_INCLUDED_3TA98U6VI7TKF22C


#include "type.hpp"
#include "structure_ref.hpp"
#include "common/override.hpp"


namespace ptrs
{
	struct structure_type : type
	{
		explicit structure_type(const structure_ref &ref);
		virtual void print_name(std::ostream &os) const PTR_SCRIPT_OVERRIDE;
		
	private:
	
		structure_ref m_ref;
	};
}


#endif
