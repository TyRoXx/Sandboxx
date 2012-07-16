#include "fs_directory.h"
#include "directory.h"
#include "http_response.h"
#include "load_file.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static bool is_last_char(const char *str, char c)
{
	const size_t len = strlen(str);
	if (len == 0)
	{
		return false;
	}
	return (str[len - 1] == c);
}

static char *path_join(const char *parent, const char *child)
{
	const bool parent_has_slash = is_last_char(parent, '/');
	const size_t total_length = strlen(parent) + strlen(child) + !parent_has_slash;
	char *joined = malloc(total_length + 1);
	if (joined)
	{
		char *j = strcpy(joined, parent);
		if (!parent_has_slash)
		{
			*j = '/';
			++j;
		}
		strcpy(j, child);
	}
	return joined;
}

static bool handle_request(const char *path, struct directory_entry_t *entry, struct http_response_t *response)
{
	const char *parent = entry->data;
	char *full_path;

	if (strchr(path, '.'))
	{
		response->status = HttpStatus_Forbidden;
		return true;
	}

	full_path = path_join(parent, path);

	if (!load_buffer_from_file_name(&response->body, full_path))
	{
		response->status = HttpStatus_NotFound;

		free(full_path);
		return true;
	}

	free(full_path);
	return true;
}

static void destroy_fs_dir(directory_entry_t *entry)
{
	free(entry->data);
}


bool initialize_file_system_directory(
	struct directory_entry_t *entry,
	const char *path
	)
{
	entry->data = strdup(path);
	if (!entry->data)
	{
		return false;
	}

	entry->handle_request = handle_request;
	entry->destroy = destroy_fs_dir;
	return true;
}
