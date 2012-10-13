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
			size_t get_current_jump_address() const;
			void return_();
			void set_from_constant(
				instruction_argument destination,
				instruction_argument constant
				);
			void set_null(
				instruction_argument destination
				);
			void set_function(
				instruction_argument destination,
				instruction_argument function_id
				);
			void copy(
				instruction_argument destination,
				instruction_argument source
				);
			void add(
				instruction_argument destination,
				instruction_argument summand
				);
			void not_(
				instruction_argument destination
				);
			void allocate(
				instruction_argument size
				);
			void deallocate(
				instruction_argument size
				);
			void call(
				instruction_argument argument_count
				);
			void jump(
				instruction_argument destination
				);
			void jump_if(
				instruction_argument destination,
				instruction_argument condition_address
				);
			void update_jump_destination(
				size_t jump_address,
				instruction_argument destination
				);

		private:

			instruction_vector &m_destination;


			void push_instruction(
				instruction const &instruction
				);
		};
	}
}


#endif
