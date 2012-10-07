#include "string.h"
#include "check.h"
#include "common/string.h"


static void test_empty_string(string_t *str)
{
	TEST_EXPECT(string_size(str) == 0);
	TEST_EXPECT(string_data(str) != 0);
	TEST_EXPECT(string_c_str(str) != 0);
	TEST_EXPECT(*string_c_str(str) == '\0');
}

void test_string(void)
{
	string_t str;
	TEST_EXPECT(string_create(&str));
	test_empty_string(&str);

	string_clear(&str);
	test_empty_string(&str);

	string_destroy(&str);

	TEST_EXPECT(string_create(&str));
	test_empty_string(&str);
	string_destroy(&str);
}
