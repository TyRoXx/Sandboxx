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
		typedef std::vector<guid> dependency_vector;
		typedef std::vector<std::unique_ptr<structure>> structure_vector;
		typedef std::vector<std::unique_ptr<method>> method_vector;
		
		
		explicit package(
			dependency_vector dependencies,
			structure_vector structures,
			method_vector free_methods
			);
		const dependency_vector &dependencies() const;
		const structure_vector &structures() const;
		const method_vector &free_methods() const;
		
	private:

		dependency_vector m_dependencies;
		structure_vector m_structures;
		method_vector m_free_methods;
	};
}


#endif
