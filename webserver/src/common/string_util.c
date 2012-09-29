#include "string_util.h"
#include <string.h>
#include <stdlib.h>


char *string_duplicate(const char *original)
{
	const size_t length = strlen(original);
	char *duplicate = malloc(length + 1);
	if (duplicate)
	{
		memcpy(duplicate, original, length + 1);
	}
	return duplicate;
}
