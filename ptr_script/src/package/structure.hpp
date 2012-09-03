#ifndef STRUCTURE_HPP_INCLUDED_1T75BLWB0QZWEJ1Z
#define STRUCTURE_HPP_INCLUDED_1T75BLWB0QZWEJ1Z


#include "method.hpp"
#include <vector>


namespace ptrs
{
	struct structure
	{
		typedef std::vector<method> method_vector;


		explicit structure(
			std::string full_name,
			method_vector methods
			);
		const std::string &full_name() const;
		const method_vector &methods() const;

	private:
	
		std::string m_full_name;
		method_vector m_methods;
	};
}


#endif
