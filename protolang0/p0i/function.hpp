#pragma once
#ifndef P0I_FUNCTION_HPP
#define P0I_FUNCTION_HPP


#include "instruction.hpp"
#include <vector>


namespace p0
{
	namespace intermediate
	{
		struct function
		{
			typedef std::vector<instruction> instruction_vector;


			function();
			explicit function(
				instruction_vector body
				);
			instruction_vector const &body() const;

		private:

			instruction_vector m_body;
		};
	}
}


#endif
