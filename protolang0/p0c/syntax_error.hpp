#pragma once
#ifndef P0C_SYNTAX_ERROR_HPP
#define P0C_SYNTAX_ERROR_HPP


#include "source_range.hpp"
#include <exception>


namespace p0
{
	struct syntax_error : std::runtime_error
	{
		explicit syntax_error(
			const std::string &message,
			source_range position
			);
		source_range const &position() const;

	private:
		source_range m_position;
	};
}


#endif
