#ifndef PACKAGE_HPP_INCLUDED_HW9D762JORR5CG8D
#define PACKAGE_HPP_INCLUDED_HW9D762JORR5CG8D


#include "structure.hpp"
#include "structure_ref.hpp"
#include "common/guid.hpp"
#include <vector>


namespace ptrs
{
	struct package
	{
		typedef std::vector<std::unique_ptr<structure>> structure_vector;
		typedef std::vector<guid> dependency_vector;
		
		
		explicit package(
			structure_vector structures,
			dependency_vector dependencies
			);
		const structure_vector &structures() const;
		const dependency_vector &dependencies() const;
		const package *find_package(package_ref ref) const;
		const structure *find_structure(const structure_ref &ref) const;
		
	private:
	
		structure_vector m_structures;
		dependency_vector m_dependencies;
	};
}


#endif
