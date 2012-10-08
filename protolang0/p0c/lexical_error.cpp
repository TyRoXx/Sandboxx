#include "lexical_error.hpp"


namespace p0
{
	lexical_error::lexical_error(
		const std::string &message,
		source_range position
		)
		: std::runtime_error(message)
		, m_position(position)
	{
	}

	source_range const &lexical_error::position() const
	{
		return m_position;
	}
}
