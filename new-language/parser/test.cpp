#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "parser.hpp"
#include <unordered_map>

BOOST_AUTO_TEST_CASE(scan_token_end_of_file)
{
	Si::memory_source<char> empty;
	boost::optional<nl::token> scanned = nl::scan_token(empty);
	BOOST_REQUIRE(scanned);
	BOOST_CHECK(nl::token_type::end_of_file == scanned->type);
	BOOST_CHECK_EQUAL("", scanned->content);
	BOOST_CHECK_EQUAL(0, scanned->position);
}

BOOST_AUTO_TEST_CASE(scan_token_sequence)
{
	std::string const input = ".,()= \t\n\"string\"identifier 123 return";
	auto const tokens =
	{
		nl::token_type::dot,
		nl::token_type::comma,
		nl::token_type::left_parenthesis,
		nl::token_type::right_parenthesis,
		nl::token_type::assignment,
		nl::token_type::space,
		nl::token_type::tab,
		nl::token_type::newline,
		nl::token_type::string,
		nl::token_type::identifier,
		nl::token_type::space,
		nl::token_type::integer,
		nl::token_type::space,
		nl::token_type::return_,
		nl::token_type::end_of_file
	};
	Si::memory_source<char> source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	for (auto const expected : tokens)
	{
		boost::optional<nl::token> const scanned = nl::scan_token(source);
		BOOST_REQUIRE(scanned);
		BOOST_CHECK_EQUAL(static_cast<int>(expected), static_cast<int>(scanned->type));
	}
	BOOST_CHECK(!Si::get(source));
}

BOOST_AUTO_TEST_CASE(scan_token_string)
{
	std::string const input = "\"abc123\\\"\\\\\"";
	Si::memory_source<char> source(boost::make_iterator_range(input.data(), input.data() + input.size()));
	boost::optional<nl::token> scanned = nl::scan_token(source);
	BOOST_REQUIRE(scanned);
	BOOST_CHECK(nl::token_type::string == scanned->type);
	BOOST_CHECK_EQUAL("abc123\"\\", scanned->content);
	BOOST_CHECK_EQUAL(0, scanned->position);

	boost::optional<nl::token> eof = nl::scan_token(source);
	BOOST_REQUIRE(eof);
	BOOST_CHECK(nl::token_type::end_of_file == eof->type);
	BOOST_CHECK_EQUAL("", eof->content);
	BOOST_CHECK_EQUAL(0, eof->position);
}
