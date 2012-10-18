#include "path.h"
#include "buffer.h"
#include "generic_vector.h"
#include "string.h"
#include "config.h"


int main(void)
{
	test_path();
	test_buffer();
	test_generic_vector();
	test_string();
	test_config();
	return 0;
}
