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
