#pragma once
#ifndef P0I_EMITTER_HPP
#define P0I_EMITTER_HPP


#include "instruction.hpp"
#include <vector>


namespace p0
{
	namespace intermediate
	{
		struct emitter
		{
			typedef std::vector<instruction> instruction_vector;


			explicit emitter(
				instruction_vector &destination
				);
			void return_();

		private:

			instruction_vector &m_destination;


			size_t push_instruction(
				instruction const &instruction
				);
		};
	}
}


#endif
