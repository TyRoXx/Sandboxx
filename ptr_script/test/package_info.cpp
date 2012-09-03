#include <boost/test/unit_test.hpp>

#include "package/package_info.hpp"


namespace ptrs
{
	BOOST_AUTO_TEST_CASE(PackageInfoTest)
	{
		const guid id;
		const std::string title = "testing";

		std::set<guid> dependencies;
		for (size_t i = 0; i < 10; ++i)
		{
			dependencies.insert(guid(std::string(guid::size * 2, '0' + i)));
		}

		package_info p(
			id,
			title,
			dependencies
			);

		BOOST_REQUIRE_EQUAL(id, p.id());
		BOOST_REQUIRE_EQUAL(title, p.title());
		BOOST_REQUIRE(dependencies == p.dependencies()); //Boost requires the operands to be ostream-able
	}
}
