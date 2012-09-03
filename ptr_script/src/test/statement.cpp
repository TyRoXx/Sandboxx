#include <boost/test/unit_test.hpp>

#include "package/block.hpp"
#include "package/jump.hpp"


namespace ptrs
{
	BOOST_AUTO_TEST_CASE(BlockBasicTest)
	{
		//endless loop
		{
			block::statement_vector statements;
			statements.push_back(std::unique_ptr<statement>(
				new jump(jump::continue_, 0)));

			block b(std::move(statements), true);

			BOOST_REQUIRE_EQUAL(b.statements().size(), 1);
			BOOST_REQUIRE(b.is_jump_target());
		}
	}
}
