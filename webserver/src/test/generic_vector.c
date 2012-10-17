#include "generic_vector.h"
#include "check.h"
#include "common/generic_vector.h"
#include <string.h>


static void test_append_range()
{
	static char const * const str_0 = "abc";
	static char const * const str_1 = "12345";

	bool success;

	WS_GEN_VECTOR(v, char);
	WS_GEN_VECTOR_CREATE(v);

	TEST_EXPECT(WS_GEN_VECTOR_EMPTY(v));
	TEST_EXPECT(WS_GEN_VECTOR_SIZE(v) == 0);
	TEST_EXPECT(!memcmp(WS_GEN_VECTOR_DATA(v), "", 0));

	WS_GEN_VECTOR_APPEND_RANGE(v, str_0, str_0 + strlen(str_0), success);
	TEST_EXPECT(success);

	TEST_EXPECT(!WS_GEN_VECTOR_EMPTY(v));
	TEST_EXPECT(WS_GEN_VECTOR_SIZE(v) == 3);
	TEST_EXPECT(!memcmp(WS_GEN_VECTOR_DATA(v), str_0, 3));

	WS_GEN_VECTOR_APPEND_RANGE(v, str_1, str_1 + strlen(str_1), success);
	TEST_EXPECT(success);

	TEST_EXPECT(!WS_GEN_VECTOR_EMPTY(v));
	TEST_EXPECT(WS_GEN_VECTOR_SIZE(v) == 8);
	TEST_EXPECT(!memcmp(WS_GEN_VECTOR_DATA(v), "abc12345", 8));

	WS_GEN_VECTOR_DESTROY(v);
}

void test_generic_vector()
{
	bool success;

	WS_GEN_VECTOR(v, int);
	WS_GEN_VECTOR_CREATE(v);

	TEST_EXPECT(WS_GEN_VECTOR_SIZE(v) == 0);
	WS_GEN_VECTOR_PUSH_BACK(v, 2, success);
	TEST_EXPECT(success);

	TEST_EXPECT(WS_GEN_VECTOR_SIZE(v) == 1);
	TEST_EXPECT(WS_GEN_VECTOR_CAPACITY(v) >= 1);

	WS_GEN_VECTOR_PUSH_BACK(v, 3, success);
	TEST_EXPECT(success);

	TEST_EXPECT(WS_GEN_VECTOR_SIZE(v) == 2);
	TEST_EXPECT(WS_GEN_VECTOR_CAPACITY(v) >= 2);

	WS_GEN_VECTOR_PUSH_BACK(v, 5, success);
	TEST_EXPECT(success);

	TEST_EXPECT(WS_GEN_VECTOR_SIZE(v) == 3);
	TEST_EXPECT(WS_GEN_VECTOR_CAPACITY(v) >= 3);

	WS_GEN_VECTOR_PUSH_BACK(v, 7, success);
	TEST_EXPECT(success);

	TEST_EXPECT(WS_GEN_VECTOR_SIZE(v) == 4);
	TEST_EXPECT(WS_GEN_VECTOR_CAPACITY(v) >= 4);

	TEST_EXPECT(WS_GEN_VECTOR_DATA(v)[0] == 2);
	TEST_EXPECT(WS_GEN_VECTOR_DATA(v)[1] == 3);
	TEST_EXPECT(WS_GEN_VECTOR_DATA(v)[2] == 5);
	TEST_EXPECT(WS_GEN_VECTOR_DATA(v)[3] == 7);

	WS_GEN_VECTOR_DESTROY(v);

	test_append_range();
}
