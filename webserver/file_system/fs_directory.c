#include "fs_directory.h"
#include "http/directory.h"
#include "http/http_response.h"
#include "common/load_file.h"
#include "common/path.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


static bool handle_request(const char *path, struct directory_entry_t *entry, struct http_response_t *response)
{
	const char *parent = entry->data;
	char *full_path;

	if (strstr(path, ".."))
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


bool initialize_file_system(
	struct directory_entry_t *entry,
	const char *args,
	const struct loadable_handler_t *handlers_begin,
	const struct loadable_handler_t *handlers_end,
	const char *current_fs_dir
	)
{
	entry->data = path_join(current_fs_dir, args);
	if (!entry->data)
	{
		return false;
	}

	entry->handle_request = handle_request;
	entry->destroy = destroy_fs_dir;
	return true;
}
