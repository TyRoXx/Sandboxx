#pragma once
#ifndef P0I_UNIT_HPP
#define P0I_UNIT_HPP


#include <vector>
#include "function.hpp"


namespace p0
{
	namespace intermediate
	{
		struct unit
		{
			typedef std::vector<function> function_vector;


			explicit unit(
				function_vector functions
				);
			function_vector const &functions() const;
			
		private:

			function_vector m_functions;
		};
	}
}


#endif
