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
				instruction_vector body,
				size_t parameters,
				size_t locals
				);
			instruction_vector const &body() const;
			size_t parameters() const;
			size_t locals() const;

		private:

			instruction_vector m_body;
			size_t m_parameters;
			size_t m_locals;
		};
	}
}


#endif
