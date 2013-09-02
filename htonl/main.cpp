#include <cstdint>
#include <ostream>
#include <climits>
#include <fstream>
#include <cstdlib>

#define BIG_ENDIAN_HOST_DETECTED 0

#define HTONL_DETECTED 1
#include <netinet/in.h>

template <class T>
struct is_non_bool_integral : std::is_integral<T>
{
};

template <>
struct is_non_bool_integral<bool> : std::false_type
{
};

template <class Integer>
typename std::enable_if<is_non_bool_integral<typename std::decay<Integer>::type>::value, void>::type
encode_big_endian_compromise(std::ostream &out, Integer value)
{
	Integer buffer;
	switch (sizeof(value))
	{
#if HTONL_DETECTED
	case 2:
		buffer = htons(value);
		break;

	case 4:
		buffer = htonl(value);
		break;
#endif

	default:
		{
#if !BIG_ENDIAN_HOST_DETECTED
			typedef typename std::make_unsigned<Integer>::type Unsigned;
			Unsigned const shiftableValue = value;
			unsigned char * const digits = reinterpret_cast<unsigned char *>(&buffer);
			for (std::size_t i = 0; i < sizeof(buffer); ++i)
			{
				digits[i] = static_cast<unsigned char>(
				             shiftableValue >> ((sizeof(buffer) - i - 1) * CHAR_BIT)
				            );
			}
#endif
			break;
		}
	}
	out.write(reinterpret_cast<char *>(&buffer), sizeof(buffer));
}

int main()
{
	std::ofstream file("out.bin", std::ios::binary);
	if (!file)
	{
		return 1;
	}
	std::uint32_t const i = std::rand();
	encode_big_endian_compromise(file, i);
}
