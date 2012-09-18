#include <vector>
#include <cassert>
#include <cstdint>

struct little_endian
{
	template <class T>
	static char get_byte(T value, std::size_t i)
	{
		return static_cast<char>(value >> (i * 8));
	}
};

struct big_endian
{
	template <class T>
	static char get_byte(T value, std::size_t i)
	{
		return static_cast<char>(value >> ((sizeof(value) - i - 1) * 8));
	}
};

struct native
{
	template <class T>
	static char get_byte(T value, std::size_t i)
	{
		return reinterpret_cast<const char *>(&value)[i];
	}
};

template <class Endianness, class T, class Out>
void write_bytes(Out dest, T value)
{
	for (std::size_t i = 0; i < sizeof(value); ++i)
	{
		*(dest++) = Endianness::get_byte(value, i);
	}
}

template <class Endianness, class T>
void overwrite_bytes(std::vector<char> &buffer, std::size_t position, T value)
{
	auto dest = buffer.begin() + position;
	write_bytes<Endianness>(dest, value);
}

template <class Endianness, class T>
void append_bytes(std::vector<char> &buffer, T value)
{
	buffer.resize(buffer.size() + sizeof(value));
	auto dest = (buffer.end() - sizeof(value));
	write_bytes<Endianness>(dest, value);
}

int main()
{
	std::vector<char> le, be, na;
	
	{
		append_bytes<little_endian, std::uint16_t>(le, 0x1234);
		assert(le[0] == '\x34');
		assert(le[1] == '\x12');
	}
	
	{
		const auto position = be.size();
		append_bytes<big_endian, std::uint16_t>(be, 0xbbaa);
		assert(be[0] == '\xbb');
		assert(be[1] == '\xaa');
		
		overwrite_bytes<big_endian, std::uint16_t>(be, position, 0x1234);
		assert(be[0] == '\x12');
		assert(be[1] == '\x34');
	}
	
	{
		append_bytes<native, std::uint16_t>(na, 0x1234);
		assert(
			(na == le) !=
			(na == be)
			);
	}
}
