#pragma once
#ifndef P0C_COMPILER_HPP
#define P0C_COMPILER_HPP


#include "p0i/unit.hpp"
#include "source_range.hpp"
#include <functional>


namespace p0
{
	struct compiler_error;


	struct compiler
	{
		typedef std::function<bool (const compiler_error &)> compiler_error_handler;


		explicit compiler(
			source_range source,
			compiler_error_handler error_handler
			);
		intermediate::unit compile();

	private:

		source_range m_source;
		compiler_error_handler const m_error_handler;
	};
}


#endif
