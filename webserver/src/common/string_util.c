#include "string_util.h"
#include <string.h>
#include <stdlib.h>


char *string_duplicate(char const *original)
{
	size_t const length = strlen(original);
	return data_duplicate(original, length + 1);
}

char *data_duplicate(char const *data, size_t size)
{
	char * const duplicate = malloc(size);
	if (duplicate)
	{
		memcpy(duplicate, data, size);
	}
	return duplicate;
}

size_t string_index_of(char const *str, char c)
{
	char const *s = str;
	while ((*s != '\0') && (*s != c))
	{
		++s;
	}
	return (size_t)(s - str);
}
