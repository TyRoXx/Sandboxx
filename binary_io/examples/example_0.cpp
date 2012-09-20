#include "binary_io/digit_iterator.hpp"
#include "binary_io/little_endian.hpp"
#include "binary_io/big_endian.hpp"
#include <iostream>
#include <array>
#include <algorithm>
#include <cstdint>
#include <iomanip>

namespace
{
	template <class Sink, class I>
	void print_hex_bytes(Sink &out, I begin, I end)
	{
		std::for_each(begin, end, [&out](unsigned char b)
		{
			out << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(b) << " ";
		});
	}
}

int main()
{
	using namespace bio;

	{
		std::uint32_t value = 0xaabbccdd;
		digit_iterator<little_endian, decltype(value)> begin(value), end;
		print_hex_bytes(std::cout, begin, end);
		std::cout << "\n";
	}

	{
		std::uint32_t value = 0xaabbccdd;
		digit_iterator<big_endian, decltype(value)> begin(value), end;
		print_hex_bytes(std::cout, begin, end);
		std::cout << "\n";
	}

	{
		std::uint32_t value = 0xaabb1122;
		digit_iterator<big_endian, decltype(value), std::uint16_t> begin(value), end;
		assert(*begin == 0xaabb);
		++begin;
		assert(*begin == 0x1122);
		++begin;
		assert(begin == end);
	}
}
