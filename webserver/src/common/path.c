#include "path.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>


static bool is_last_char(char const *str, char c)
{
	const size_t len = strlen(str);
	if (len == 0)
	{
		return false;
	}
	return (str[len - 1] == c);
}

char *path_join(char const *parent, char const *child)
{
	const bool parent_has_slash = is_last_char(parent, '/');
	const bool child_has_slash = (*child == '/');
	const size_t parent_length = strlen(parent);
	const size_t child_length = strlen(child);
	const bool do_insert_slash =
		(parent_length > 0) && (child_length > 0) &&
		!parent_has_slash && !child_has_slash;
	const size_t total_length = (parent_length + child_length + do_insert_slash);
	char * const joined = malloc(total_length + 1);
	if (joined)
	{
		char *dest = joined;

		memcpy(joined, parent, parent_length);
		dest += parent_length;

		if (do_insert_slash)
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
