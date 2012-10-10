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

		void emitter::set_from_constant(
			instruction_argument destination,
			instruction_argument constant
			)
		{
			push_instruction(instruction(
				instruction_type::set_from_constant,
				destination,
				constant
				));
		}

		void emitter::set_null(
			instruction_argument destination
			)
		{
			push_instruction(instruction(
				instruction_type::set_null,
				destination
				));
		}

		void emitter::copy(
			instruction_argument destination,
			instruction_argument source
			)
		{
			push_instruction(instruction(
				instruction_type::copy,
				destination,
				source
				));
		}

		void emitter::add(
			instruction_argument destination,
			instruction_argument summand
			)
		{
			push_instruction(instruction(
				instruction_type::add,
				destination,
				summand
				));
		}

		void emitter::allocate(
			instruction_argument size
			)
		{
			push_instruction(instruction(
				instruction_type::allocate,
				size
				));
		}

		void emitter::deallocate(
			instruction_argument size
			)
		{
			push_instruction(instruction(
				instruction_type::deallocate,
				size
				));
		}


		void emitter::push_instruction(
			instruction const &instruction
			)
		{
			m_destination.push_back(instruction);
		}
	}
}
