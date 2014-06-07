#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "parser.hpp"

BOOST_AUTO_TEST_CASE(scan_token_end_of_file)
{
	Si::memory_source<char> empty;
	boost::optional<nl::token> scanned = nl::scan_token(empty);
	BOOST_REQUIRE(scanned);
	BOOST_CHECK(nl::token_type::end_of_file == scanned->type);
	BOOST_CHECK_EQUAL("", scanned->content);
	BOOST_CHECK_EQUAL(0, scanned->position);
}
