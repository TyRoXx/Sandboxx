#include <boost/test/unit_test.hpp>

#include <set>
#include <sstream>

#include "common/guid_generator.hpp"


namespace ptrs
{
	BOOST_AUTO_TEST_CASE(GUIDTest)
	{
		guid g;
		BOOST_REQUIRE(is_zero(g));
		BOOST_REQUIRE_EQUAL(g.elements.size(), guid::size);
		BOOST_REQUIRE_EQUAL(guid::size, 20);
		BOOST_REQUIRE_EQUAL(sizeof(g.elements[0]), sizeof(guid_byte));
		BOOST_REQUIRE_EQUAL(sizeof(char), sizeof(guid_byte));
		BOOST_REQUIRE_GE(sizeof(guid), guid::size);
	}

	BOOST_AUTO_TEST_CASE(GUIDToStringTest)
	{
		const guid zero;
		BOOST_REQUIRE_EQUAL(to_string(zero), std::string(guid::size * 2, '0'));
	}

	BOOST_AUTO_TEST_CASE(GUIDGeneratorTest)
	{
		guid_generator gen;
		std::mt19937 rng;

		static const size_t Count = 1000;

		std::set<guid> guids;
		std::generate_n(
			std::inserter(guids, guids.end()),
			Count,
			[&]() -> guid
		{
			return gen(rng);
		});

		BOOST_REQUIRE_EQUAL(guids.size(), Count);
	}
}
