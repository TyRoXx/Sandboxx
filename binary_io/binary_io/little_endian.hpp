#ifndef BINARY_IO_LITTLE_ENDIAN_HPP_
#define BINARY_IO_LITTLE_ENDIAN_HPP_


#include <cstddef>


namespace bio
{
	struct little_endian
	{
		static inline std::size_t get_shift(std::size_t position, std::size_t value_size)
		{
			return position;
		}
	};
}


#endif
