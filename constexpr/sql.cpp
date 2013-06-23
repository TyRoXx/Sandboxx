#include <iostream>
#include <boost/static_assert.hpp>

constexpr bool is_whitespace(char c)
{
	return (c != '\0') && (c <= ' ');
}

BOOST_STATIC_ASSERT(is_whitespace(' '));
BOOST_STATIC_ASSERT(is_whitespace('\n'));
BOOST_STATIC_ASSERT(is_whitespace('\t'));
BOOST_STATIC_ASSERT(is_whitespace('\r'));
BOOST_STATIC_ASSERT(!is_whitespace('\0'));
BOOST_STATIC_ASSERT(!is_whitespace('a'));
BOOST_STATIC_ASSERT(!is_whitespace('0'));
BOOST_STATIC_ASSERT(!is_whitespace(127));

constexpr char const *eat_whitespace(char const *str)
{
	return is_whitespace(*str) ? eat_whitespace(str + 1) : str;
}

constexpr bool begins_with(char const *input, char const *word)
{
	return (*word == '\0') || ((*word == *input) && begins_with(input + 1, word + 1));
}

BOOST_STATIC_ASSERT(begins_with("a", ""));
BOOST_STATIC_ASSERT(begins_with("a", "a"));
BOOST_STATIC_ASSERT(begins_with("ab", ""));
BOOST_STATIC_ASSERT(begins_with("ab", "a"));
BOOST_STATIC_ASSERT(begins_with("ab", "ab"));
BOOST_STATIC_ASSERT(begins_with("", ""));
BOOST_STATIC_ASSERT(!begins_with("", "a"));
BOOST_STATIC_ASSERT(!begins_with("", "ab"));
BOOST_STATIC_ASSERT(!begins_with("a", "ab"));
BOOST_STATIC_ASSERT(!begins_with("ab", "b"));

struct parse_result
{
	bool success;
	char const *pos;
};

constexpr parse_result check_not_empty(char const *str)
{
	return {*str != '\0', str};
}

constexpr parse_result parse_expression(char const *str)
{
	return check_not_empty(str);
}

constexpr bool is_sql_no_ws(char const *str)
{
	return begins_with(str, "SELECT") && parse_expression(str + 6).success;
}

constexpr bool is_sql(char const *str)
{
	return is_sql_no_ws(eat_whitespace(str));
}

BOOST_STATIC_ASSERT(is_sql("SELECT 1"));
BOOST_STATIC_ASSERT(!is_sql("SELECT"));

int main()
{
}
