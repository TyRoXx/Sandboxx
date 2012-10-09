#pragma once
#ifndef P0I_INSTRUCTION_HPP
#define P0I_INSTRUCTION_HPP


#include <array>
#include <cstdint>


namespace p0
{
	namespace intermediate
	{
		namespace instruction_type
		{
			enum Enum
			{
				nothing,
				count_,
			};
		};

		typedef std::uint_fast64_t instruction_argument;


		struct instruction
		{
			enum
			{
				max_arguments = 3
			};

			typedef std::array<instruction_argument, max_arguments> argument_array;


			instruction();
			explicit instruction(
				instruction_type::Enum type,
				argument_array const &arguments
				);
			instruction_type::Enum type() const;
			argument_array const &arguments() const;

		private:

			instruction_type::Enum m_type;
			argument_array m_arguments;
		};
	}
}


#endif
