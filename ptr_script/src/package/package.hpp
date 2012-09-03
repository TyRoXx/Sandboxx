#ifndef PACKAGE_HPP_INCLUDED_HW9D762JORR5CG8D
#define PACKAGE_HPP_INCLUDED_HW9D762JORR5CG8D


#include "structure.hpp"
#include <vector>


namespace ptrs
{
	struct package
	{
		typedef std::vector<structure> structure_vector;
		
		
		explicit package(
			structure_vector structures
			);
		const structure_vector &structures() const;
		
	private:
	
		structure_vector m_structures;
	};
}


#endif
