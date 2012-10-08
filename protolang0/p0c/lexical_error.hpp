#pragma once
#ifndef P0C_LEXICAL_ERROR_HPP
#define P0C_LEXICAL_ERROR_HPP


#include "source_range.hpp"
#include <exception>


namespace p0
{
	struct lexical_error : std::runtime_error
	{
		explicit lexical_error(
			const std::string &message,
			source_range position
			);
		source_range const &position() const;

	private:
		source_range m_position;
	};
}


#endif
