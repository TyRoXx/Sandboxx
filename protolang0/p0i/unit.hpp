#pragma once
#ifndef P0I_UNIT_HPP
#define P0I_UNIT_HPP


#include <string>


namespace p0
{
	namespace intermediate
	{
		struct unit
		{
			explicit unit(
				std::string name
				);
			const std::string &name() const;
			
		private:

			std::string m_name;
		};
	}
}


#endif
