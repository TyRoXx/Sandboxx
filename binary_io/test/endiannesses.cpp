#include <boost/test/unit_test.hpp>

#include "binary_io/little_endian.hpp"
#include "binary_io/big_endian.hpp"
#include "binary_io/native_endianness.hpp"

#include <algorithm>
#include <cstdint>

using namespace bio;


BOOST_AUTO_TEST_CASE(LittleEndianTest)
{
	BOOST_CHECK_EQUAL(little_endian::get_shift(0, 4), 0);
	BOOST_CHECK_EQUAL(little_endian::get_shift(1, 4), 1);
	BOOST_CHECK_EQUAL(little_endian::get_shift(2, 4), 2);
	BOOST_CHECK_EQUAL(little_endian::get_shift(3, 4), 3);
}

BOOST_AUTO_TEST_CASE(BigEndianTest)
{
	BOOST_CHECK_EQUAL(big_endian::get_shift(0, 4), 3);
	BOOST_CHECK_EQUAL(big_endian::get_shift(1, 4), 2);
	BOOST_CHECK_EQUAL(big_endian::get_shift(2, 4), 1);
	BOOST_CHECK_EQUAL(big_endian::get_shift(3, 4), 0);
}

BOOST_AUTO_TEST_CASE(NativeEndianTest)
{
	static const std::uint32_t Value = 0xaabbccdd;
	const unsigned char * const ValueAsBytes = reinterpret_cast<const unsigned char *>(&Value);

	const auto index_of = [ValueAsBytes](unsigned char digit) -> std::size_t
	{
		return std::distance(
			ValueAsBytes,
			std::find(ValueAsBytes, ValueAsBytes + sizeof(Value), digit)
			);
	};

	BOOST_CHECK_EQUAL(native_endianness::get_shift(0, 4), index_of(0xdd));
	BOOST_CHECK_EQUAL(native_endianness::get_shift(1, 4), index_of(0xcc));
	BOOST_CHECK_EQUAL(native_endianness::get_shift(2, 4), index_of(0xbb));
	BOOST_CHECK_EQUAL(native_endianness::get_shift(3, 4), index_of(0xaa));
}
