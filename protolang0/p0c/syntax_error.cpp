#include "syntax_error.hpp"


namespace p0
{
	syntax_error::syntax_error(
		const std::string &message,
		source_range position
		)
		: std::runtime_error(message)
		, m_position(position)
	{
	}

	source_range const &syntax_error::position() const
	{
		return m_position;
	}
}
