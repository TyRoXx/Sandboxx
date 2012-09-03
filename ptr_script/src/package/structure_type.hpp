#ifndef STRUCTURE_TYPE_HPP_INCLUDED_3TA98U6VI7TKF22C
#define STRUCTURE_TYPE_HPP_INCLUDED_3TA98U6VI7TKF22C


#include "type.hpp"
#include "structure_ref.hpp"


namespace ptrs
{
	struct structure_type : type
	{
		explicit structure_type(const structure_ref &ref);
		const structure_ref &ref() const;

	private:
	
		structure_ref m_ref;
	};
}


#endif
