#include "instruction.hpp"


namespace p0
{
	namespace intermediate
	{
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
