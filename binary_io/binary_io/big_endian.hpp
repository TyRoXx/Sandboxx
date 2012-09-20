#ifndef BINARY_IO_BIG_ENDIAN_HPP_
#define BINARY_IO_BIG_ENDIAN_HPP_


#include <cstddef>


namespace bio
{
	struct big_endian
	{
		static inline std::size_t get_shift(std::size_t position, std::size_t value_size)
		{
			return (value_size - position - 1);
		}
	};
}


#endif
