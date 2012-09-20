#include <boost/test/unit_test.hpp>

#include "binary_io/digit_iterator.hpp"
#include "binary_io/little_endian.hpp"
#include "binary_io/big_endian.hpp"

#include <cstdint>

using namespace bio;


BOOST_AUTO_TEST_CASE(DigitIteratorTest)
{
	{
		std::uint32_t value = 0xaabbccdd;
		digit_iterator<little_endian, decltype(value)> begin(value), end;
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xdd);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xcc);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xbb);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xaa);
		BOOST_CHECK(begin == end);
	}

	{
		std::uint32_t value = 0xaabbccdd;
		digit_iterator<big_endian, decltype(value)> begin(value), end;
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xaa);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xbb);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xcc);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xdd);
		BOOST_CHECK(begin == end);
	}

	{
		std::uint32_t value = 0xaabb1122;
		digit_iterator<big_endian, decltype(value), std::uint16_t> begin(value), end;
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xaabb);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0x1122);
		BOOST_CHECK(begin == end);
	}

	{
		std::uint64_t value = 0xaabbccdd11223344;
		digit_iterator<big_endian, decltype(value), std::uint16_t> begin(value), end;
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xaabb);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0xccdd);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0x1122);
		BOOST_CHECK(begin != end);
		BOOST_CHECK_EQUAL(*begin++, 0x3344);
		BOOST_CHECK(begin == end);
	}
}
