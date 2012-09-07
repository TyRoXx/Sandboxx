#include "source.hpp"


namespace ptrs
{
	namespace serialization
	{
		parse_error::parse_error(const std::string &message)
			: std::runtime_error(message)
		{
		}
		
		
		source::~source()
		{
		}
	}
}
