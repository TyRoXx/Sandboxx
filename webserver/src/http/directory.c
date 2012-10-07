#include "directory.h"
#include "http_response.h"
#include "common/imstream.h"
#include "common/buffer.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>


void directory_entry_destroy(directory_entry_t *entry)
{
	free(entry->name);
	entry->destroy(entry);
}


void directory_create(directory_t *directory)
{
	directory->entries = 0;
	directory->entry_count = 0;
	directory->default_ = 0;
}

void directory_destroy(directory_t *directory)
{
	size_t i;

	for (i = 0; i < directory->entry_count; ++i)
	{
		directory_entry_t *entry = directory->entries + i;
		directory_entry_destroy(entry);
	}

	if (directory->default_)
	{
		directory_entry_destroy(directory->default_);
		free(directory->default_);
	}
}

static char const * const PathSeparators = "/?";

static char const *find_path_separator(char const *path)
{
	while (*path &&
		!strchr((char *)PathSeparators, *path))
	{
		++path;
	}

	return path;
}

static bool name_equals(char const *name, char const *begin, char const *end)
{
	const size_t name_length = strlen(name);

	return
		(name_length == (end - begin) &&
		!memcmp(name, begin, name_length));
}

static char const NotFoundMessage[] = "Not found";

static bool set_not_found_message(struct http_response_t *response)
{
	size_t const message_len = strlen(NotFoundMessage);
	if (!imstream_create(&response->body, NotFoundMessage, message_len))
	{
		return false;
	}

	response->body_size = message_len;
	function_set_nothing(&response->destroy_body);
	return true;
}

bool directory_handle_request(const directory_t *directory, char const *path, struct http_response_t *response)
{
	char const * const part_end = find_path_separator(path);
	char const * sub_path = part_end;
	size_t i;

	if (*sub_path == '/')
	{
		++sub_path;
	}
	
	for (i = 0; i < directory->entry_count; ++i)
	{
		directory_entry_t * const entry = directory->entries + i;

		if (name_equals(entry->name, path, part_end))
		{
			return entry->handle_request(
				sub_path,
				entry,
				response);
		}
	}

	if (directory->default_)
	{
		return directory->default_->handle_request(
			path,
			directory->default_,
			response);
	}

	response->status = HttpStatus_NotFound;
	return set_not_found_message(response);
}
