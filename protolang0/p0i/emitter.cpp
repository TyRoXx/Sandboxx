#include "emitter.hpp"


namespace p0
{
	namespace intermediate
	{
		emitter::emitter(
			instruction_vector &destination
			)
			: m_destination(destination)
		{
		}

		void emitter::return_()
		{
			push_instruction(instruction(
				instruction_type::return_
				));
		}


		size_t emitter::push_instruction(
			instruction const &instruction
			)
		{
			auto const address = m_destination.size();
			m_destination.push_back(instruction);
			return address;
		}
	}
}
