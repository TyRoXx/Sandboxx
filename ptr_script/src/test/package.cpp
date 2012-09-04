#include <boost/test/unit_test.hpp>

#include "package/package.hpp"


namespace ptrs
{
	BOOST_AUTO_TEST_CASE(PackageBasicTest)
	{
		package::structure_vector structs;
		package::dependency_vector deps;
		deps.push_back(guid(std::string(guid::size * 2, 'F')));

		package p(
			std::move(structs),
			std::move(deps));

		BOOST_REQUIRE_EQUAL(p.structures().size(), 0);
		BOOST_REQUIRE_EQUAL(p.dependencies().size(), 1);
	}
}
