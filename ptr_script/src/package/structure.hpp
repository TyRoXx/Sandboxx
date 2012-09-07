#ifndef STRUCTURE_HPP_INCLUDED_1T75BLWB0QZWEJ1Z
#define STRUCTURE_HPP_INCLUDED_1T75BLWB0QZWEJ1Z


#include "method.hpp"
#include "element.hpp"
#include <vector>


namespace ptrs
{
	struct structure
	{
		typedef std::vector<std::unique_ptr<method>> method_vector;
		typedef std::vector<std::unique_ptr<element>> element_vector;


		explicit structure(
			std::string full_name,
			method_vector methods,
			element_vector elements
			);
		const std::string &full_name() const;
		const method_vector &methods() const;
		const element_vector &elements() const;

	private:
	
		std::string m_full_name;
		method_vector m_methods;
		element_vector m_elements;
	};
}


#endif
