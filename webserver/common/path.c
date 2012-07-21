#include "path.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>


static bool is_last_char(const char *str, char c)
{
	const size_t len = strlen(str);
	if (len == 0)
	{
		return false;
	}
	return (str[len - 1] == c);
}

char *path_join(const char *parent, const char *child)
{
	const bool parent_has_slash =
		is_last_char(parent, '/') ||
		(*parent == '\0');
	const size_t parent_length = strlen(parent);
	const size_t child_length = strlen(child);
	const size_t total_length = parent_length + child_length + !parent_has_slash;
	char *joined = malloc(total_length + 1);
	if (joined)
	{
		char *dest = joined;
		memcpy(joined, parent, parent_length);
		dest += parent_length;

		if (!parent_has_slash)
		{
			*dest = '/';
			++dest;
		}

		memcpy(dest, child, child_length);
		dest += child_length;

		*dest = '\0';

		assert(dest == joined + total_length);
	}
	return joined;
}
