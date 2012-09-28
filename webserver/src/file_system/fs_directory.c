#include "fs_directory.h"
#include "http/directory.h"
#include "http/http_response.h"
#include "common/path.h"
#include "common/ifstream.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


static void close_file(void *file)
{
	fclose(file);
}

static bool handle_request(const char *path, struct directory_entry_t *entry, struct http_response_t *response)
{
	const char * const parent = entry->data;
	char *full_path;
	FILE *file;

	if (strstr(path, ".."))
	{
		response->status = HttpStatus_Forbidden;
		return true;
	}

	full_path = path_join(parent, path);

	file = fopen(full_path, "rb");
	if (!file)
	{
		response->status = HttpStatus_NotFound;

		free(full_path);

		function_set_nothing(&response->destroy_body);
		return true;
	}

	free(full_path);

	if (!ifstream_create(&response->body, file))
	{
		response->status = HttpStatus_InternalServerError;

		function_set_nothing(&response->destroy_body);
		return true;
	}

	function_create(&response->destroy_body, close_file, file);

	fseek(file, 0, SEEK_END);
	response->body_size = ftell(file);
	fseek(file, 0, SEEK_SET);
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
