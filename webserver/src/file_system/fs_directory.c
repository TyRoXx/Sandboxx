#include "fs_directory.h"
#include "http/directory.h"
#include "http/http_response.h"
#include "common/path.h"
#include "common/ifstream.h"
#include "common/imstream.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


static void close_file(void *file)
{
	fclose(file);
}

static bool handle_request(char const *path, struct directory_entry_t *entry, struct http_response_t *response)
{
	char const * const parent = entry->data;
	char *full_path;
	FILE *file;
	char const *error_message;

	if (strstr(path, ".."))
	{
		response->status = HttpStatus_Forbidden;
		error_message = "Forbidden";
		goto on_error;
	}

	full_path = path_join(parent, path);

	file = fopen(full_path, "rb");
	if (!file)
	{
		response->status = HttpStatus_NotFound;
		error_message = "Not found";

		free(full_path);
		goto on_error;
	}

	free(full_path);

	if (!ifstream_create(&response->body, file))
	{
		response->status = HttpStatus_InternalServerError;
		error_message = "Internal server error";
		goto on_error;
	}

	function_create(&response->destroy_body, close_file, file);

	fseek(file, 0, SEEK_END);
	response->body_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return true;

on_error:
	{
		const size_t error_message_length = strlen(error_message);

		imstream_create(&response->body, error_message, error_message_length);
		response->body_size = error_message_length;
	}
	function_set_nothing(&response->destroy_body);
	return true;
}

static void destroy_fs_dir(directory_entry_t *entry)
{
	free(entry->data);
}


bool initialize_file_system(
	struct directory_entry_t *entry,
	char const *args,
	const struct loadable_handler_t *handlers_begin,
	const struct loadable_handler_t *handlers_end,
	char const *current_fs_dir,
	void const *data
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
