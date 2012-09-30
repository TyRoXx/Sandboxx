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

size_t string_index_of(const char * const str, const char c)
{
	const char *s = str;
	while ((*s != '\0') && (*s != c))
	{
		++s;
	}
	return (size_t)(s - str);
}
