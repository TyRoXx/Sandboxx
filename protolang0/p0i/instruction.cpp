#include "instruction.hpp"


namespace p0
{
	namespace intermediate
	{
		instruction_info const &get_instruction_info(instruction_type::Enum instruction)
		{
			static std::array<instruction_info, instruction_type::count_> const infos =
			{{
				{"nothing", 0},
				{"return", 0},
				{"allocate", 1},
				{"deallocate", 1},
				{"set_from_constant", 2},
				{"set_null", 1},
				{"set_function", 2},
				{"copy", 2},
				{"add", 2},
				{"mul", 2},
				{"div", 2},
				{"negate", 1},
				{"not", 1},
				{"and", 2},
				{"or", 2},
				{"xor", 2},
				{"equal", 2},
				{"less", 2},
				{"greater", 2},
				{"call", 1},
			}};

			return infos[instruction];
		}


		instruction::instruction()
			: m_type(instruction_type::nothing)
		{
		}

		instruction::instruction(
			instruction_type::Enum type,
			argument_array const &arguments
			)
			: m_type(type)
			, m_arguments(arguments)
		{
		}

		instruction::instruction(
			instruction_type::Enum type,
			instruction_argument argument0
			)
			: m_type(type)
		{
			m_arguments[0] = argument0;
		}

		instruction::instruction(
			instruction_type::Enum type,
			instruction_argument argument0,
			instruction_argument argument1
			)
			: m_type(type)
		{
			m_arguments[0] = argument0;
			m_arguments[1] = argument1;
		}

		instruction_type::Enum instruction::type() const
		{
			return m_type;
		}

		instruction::argument_array const &instruction::arguments() const
		{
			return m_arguments;
		}
	}
}
