#include "config.h"
#include "check.h"
#include "common/config.h"


#if (defined WS_WIN32 == defined WS_UNIX)
#  error either WS_WIN32 or WS_UNIX is expected to be defined
#endif

#ifdef WS_WIN32
#  ifndef _WIN32
#    error WS_WIN32 is only expected to be defined when _WIN32 is defined, too
#  endif
#endif


void test_config(void)
{
	/* just to make sure our custom bool type works like in C++*/
	
	bool t = true;
	bool f = false;
	TEST_EXPECT(t);
	TEST_EXPECT(!f);

	TEST_EXPECT(t != f);
	TEST_EXPECT(t || f);
	TEST_EXPECT(t && !f);

	TEST_EXPECT(t ^ f);
	TEST_EXPECT(t | f);
	TEST_EXPECT(t & !f);

	TEST_EXPECT(true == 1);
	TEST_EXPECT(false == 0);
	TEST_EXPECT(true);
	TEST_EXPECT(!false);
	TEST_EXPECT(!!true);
}
