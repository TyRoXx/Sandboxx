#include "path.h"
#include "common/path.h"
#include <stdio.h>
#include <string.h>


#define TEST_EXPECT(x) if (!(x)) { fprintf(stderr, "Failed (%s:%u): %s\n", __FILE__, __LINE__, #x); }


static void check_join_equal(
	const char *left,
	const char *right,
	const char *joined)
{
	char *result = path_join(left, right);
	TEST_EXPECT(result);
	if (result)
	{
		TEST_EXPECT(!strcmp(result, joined));
	}
}

void test_path()
{
	check_join_equal(".", "abc", "./abc");
	check_join_equal("parent", "child", "parent/child");
}
