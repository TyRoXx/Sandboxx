#include <boost/test/unit_test.hpp>

#include "package/structure_type.hpp"


namespace ptrs
{
	BOOST_AUTO_TEST_CASE(TypeBasicTest)
	{
		const structure_ref ref(package_ref(package_ref::self_index), 0);
		structure_type s(ref);

		BOOST_REQUIRE(s.ref() == ref);
	}
}
