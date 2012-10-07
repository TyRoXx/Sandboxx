#include "buffer.h"
#include "check.h"
#include "common/buffer.h"
#include <string.h>


static void test_buffer_size(size_t size)
{
	buffer_t buffer;
	buffer_create(&buffer);

	TEST_EXPECT(buffer.size == 0);
	TEST_EXPECT(buffer.capacity == 0);

	TEST_EXPECT(buffer_resize(&buffer, size));
	TEST_EXPECT(buffer.size == size);
	TEST_EXPECT(buffer.capacity >= size);

	buffer_destroy(&buffer);
}

static void test_buffer_sizes(void)
{
	size_t i;
	for (i = 0; i < 256; ++i)
	{
		test_buffer_size(i);
	}
}

void test_buffer(void)
{
	buffer_t buffer;
	buffer_create(&buffer);
	TEST_EXPECT(buffer.size == 0);
	
	TEST_EXPECT(buffer_append(&buffer, "hallo", 6));
	TEST_EXPECT(buffer.size == 6);
	TEST_EXPECT(buffer.capacity >= 6);
	TEST_EXPECT(!strcmp(buffer.data, "hallo"));
	TEST_EXPECT(buffer_resize(&buffer, 0));
	TEST_EXPECT(buffer.size == 0);
	TEST_EXPECT(buffer_push_back(&buffer, '1'));
	TEST_EXPECT(buffer_push_back(&buffer, '2'));
	TEST_EXPECT(buffer_push_back(&buffer, '3'));
	TEST_EXPECT(buffer_push_back(&buffer, '\0'));
	TEST_EXPECT(buffer.size == 4);
	TEST_EXPECT(buffer_resize(&buffer, 4));
	TEST_EXPECT(buffer.size == 4);
	TEST_EXPECT(!strcmp(buffer.data, "123"));

	buffer_destroy(&buffer);

	test_buffer_sizes();
}
