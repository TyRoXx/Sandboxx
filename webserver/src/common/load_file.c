#include "load_file.h"
#include <stdio.h>


bool load_buffer_from_file_name(buffer_t *content, char const *file_name)
{
	static const size_t Growth = 4096;
	size_t total_read = 0;
	FILE * const file = fopen(file_name, "rb");
	if (!file)
	{
		return false;
	}

	for (;;)
	{
		size_t just_read;

		if (!buffer_resize(content, total_read + Growth))
		{
			fclose(file);
			return 0;
		}

		just_read = fread(content->data + total_read, 1, Growth, file);
		if (just_read == 0)
		{
			break;
		}

		total_read += just_read;
	}

	buffer_resize(content, total_read);
	fclose(file);
	return true;
}
