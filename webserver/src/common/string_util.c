#include "string_util.h"
#include <string.h>
#include <stdlib.h>


char *string_duplicate(const char *original)
{
	const size_t length = strlen(original);
	return data_duplicate(original, length + 1);
}

char *data_duplicate(const char *data, size_t size)
{
	char *duplicate = malloc(size);
	if (duplicate)
	{
		memcpy(duplicate, data, size);
	}
	return duplicate;
}
