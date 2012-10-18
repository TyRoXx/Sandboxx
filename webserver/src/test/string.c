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

static void test_append_c_str()
{
	string_t s;
	TEST_EXPECT(string_create(&s));
	TEST_EXPECT(string_append_c_str(&s, ""));
	TEST_EXPECT(!strcmp(string_c_str(&s), ""));
	TEST_EXPECT(string_size(&s) == 0);

	TEST_EXPECT(string_append_c_str(&s, "abc"));
	TEST_EXPECT(!strcmp(string_c_str(&s), "abc"));
	TEST_EXPECT(string_size(&s) == 3);

	TEST_EXPECT(string_append_c_str(&s, "123"));
	TEST_EXPECT(!strcmp(string_c_str(&s), "abc123"));
	TEST_EXPECT(string_size(&s) == 6);

	string_destroy(&s);
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

	test_append_c_str();
}
