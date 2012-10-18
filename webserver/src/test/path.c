#include "path.h"
#include "check.h"
#include "common/path.h"
#include <string.h>
#include <stdlib.h>


static void check_join_equal(
	char const *left,
	char const *right,
	char const *joined)
{
	char *result = path_join(left, right);
	TEST_EXPECT(result);
	if (result)
	{
		TEST_EXPECT(!strcmp(result, joined));
	}
	free(result);
}

void test_path()
{
	check_join_equal(".", "abc", "./abc");
	check_join_equal("parent", "child", "parent/child");
	check_join_equal("parent/", "child", "parent/child");
	check_join_equal("parent/child", "", "parent/child");
	check_join_equal("", "parent/child", "parent/child");
	check_join_equal("parent", "/child", "parent/child");
	check_join_equal("/", "child", "/child");
}
