#include "sub_directory.h"
#include "http/directory.h"
#include "http/load_directory.h"
#include "common/buffer.h"
#include "common/load_file.h"
#include "common/path.h"
#include <stdlib.h>
#include <stdio.h>


static bool handle_request(char const *path, struct directory_entry_t *entry, struct http_response_t *response)
{
	return directory_handle_request(entry->data, path, response);
}

static void destroy_sub_directory(directory_entry_t *entry)
{
	directory_destroy(entry->data);
	free(entry->data);
}


bool initialize_sub_directory(
	directory_entry_t *entry,
	char const *args,
	const loadable_handler_t *handlers_begin,
	const loadable_handler_t *handlers_end,
	char const *current_fs_dir,
	void const *data
	)
{
	buffer_t description;
	char *sub_fs_dir;
	char *description_file_name;

	entry->data = malloc(sizeof(directory_t));
	if (!entry->data)
	{
		return false;
	}

	sub_fs_dir = path_join(current_fs_dir, args);
	if (!sub_fs_dir)
	{
		return false;
	}

	description_file_name = path_join(sub_fs_dir, "directory.txt");
	if (!description_file_name)
	{
		free(sub_fs_dir);
		return false;
	}

	directory_create(entry->data);
	buffer_create(&description);

	if (!load_buffer_from_file_name(&description, description_file_name))
	{
		fprintf(stderr, "Could not load directory description '%s'", description_file_name);
		buffer_destroy(&description);
		free(description_file_name);
		return false;
	}

	free(description_file_name);

	if (!load_directory(
		entry->data,
		description.data,
		description.data + description.size,
		handlers_begin,
		handlers_end,
		sub_fs_dir))
	{
		fprintf(stderr, "Could not parse directory description %s", args);
		buffer_destroy(&description);
		free(sub_fs_dir);
		return false;
	}

	entry->handle_request = handle_request;
	entry->destroy = destroy_sub_directory;
	free(sub_fs_dir);
	return true;
}
