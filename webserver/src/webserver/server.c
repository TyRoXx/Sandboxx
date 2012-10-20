#include "server.h"
#include "client.h"
#include "common/path.h"
#include "common/thread.h"
#include "common/buffer.h"
#include "common/load_file.h"
#include "common/string_util.h"
#include "http/http_response.h"
#include "http/http_request.h"
#include "file_system/fs_directory.h"
#include "lua_script/lua_script.h"
#include "sub_directory/sub_directory.h"
#include <stdio.h>


static bool load_location(
	host_entry_t *loc,
	request_handler_manager_t const *handlers,
	char const *host,
	char const *path)
{
	char * const directory_file_name = path_join(path, "directory.txt");
	buffer_t dir_file;

	assert(loc);
	assert(host);
	assert(path);

	buffer_create(&dir_file);

	if (!load_buffer_from_file_name(&dir_file, directory_file_name))
	{
		fprintf(stderr, "Could not load directory file '%s'\n", directory_file_name);
		buffer_destroy(&dir_file);
		free(directory_file_name);
		return false;
	}

	free(directory_file_name);

	directory_create(&loc->directory);

	if (!load_directory(
		&loc->directory,
		dir_file.data,
		dir_file.data + dir_file.size,
		WS_GEN_VECTOR_BEGIN(handlers->handlers),
		WS_GEN_VECTOR_END(handlers->handlers),
		path))
	{
		fprintf(stderr, "Could not parse directory file\n");
		buffer_destroy(&dir_file);
		return false;
	}

	buffer_destroy(&dir_file);

	loc->host = string_duplicate(host);
	return (loc->host != 0);
}

static void destroy_locations(host_entry_t *locations_begin, host_entry_t *locations_end)
{
	for (; locations_begin != locations_end; ++locations_begin)
	{
		host_entry_destroy(locations_begin);
	}
}

static node_plugin_t *load_node_plugin(
	char const *file_name,
	node_plugin_manager_t *plugins
	)
{
	node_plugin_t plugin;
	bool success;

	if (!node_plugin_load(&plugin, file_name))
	{
		return 0;
	}

	WS_GEN_VECTOR_PUSH_BACK(plugins->plugins, plugin, success);
	if (success)
	{
		return &WS_GEN_VECTOR_BACK(plugins->plugins);
	}

	return 0;
}

static bool plugin_entry_handle_request(
	char const *url,
	directory_entry_t *entry,
	http_response_t *response)
{
	node_plugin_t const * const plugin = entry->data;
	http_request_t const request = {"GET", (char *)url, "host"};

	return node_plugin_handle_request(
		plugin,
		&request,
		response
		);
}

static void plugin_entry_destroy(
	directory_entry_t *entry)
{
}

static bool initialize_plugin_directory_entry(
struct directory_entry_t *entry,
	char const *args,
	const struct loadable_handler_t *handlers_begin,
	const struct loadable_handler_t *handlers_end,
	char const *current_fs_dir,
	void const *data
	)
{
	node_plugin_t const * const plugin = data;

	entry->handle_request = plugin_entry_handle_request;
	entry->destroy = plugin_entry_destroy;
	entry->data = (void *)plugin;

	return true;
}

static loadable_handler_t create_plugin_handler(
	node_plugin_t const *plugin)
{
	loadable_handler_t const result =
	{
		plugin->name,
		initialize_plugin_directory_entry,
		plugin
	};
	return result;
}

static const loadable_handler_t builtin_handlers[] =
{
	{"lua", initialize_lua_script, 0},
	{"fs", initialize_file_system, 0},
	{"dir", initialize_sub_directory, 0},
};

static bool load_request_handler_plugins(
	settings_t const *settings,
	node_plugin_manager_t *plugins,
	request_handler_manager_t *handlers
	)
{
	bool success;
	char **begin = WS_GEN_VECTOR_BEGIN(settings->plugin_file_names);
	char ** const end = WS_GEN_VECTOR_END(settings->plugin_file_names);

	for (; begin != end; ++begin)
	{
		node_plugin_t * const plugin = load_node_plugin(
			*begin,
			plugins
			);
		loadable_handler_t handler;
		bool success;

		if (!plugin)
		{
			return false;
		}

		handler = create_plugin_handler(plugin);
		WS_GEN_VECTOR_PUSH_BACK(handlers->handlers, handler, success);
		if (!success)
		{
			return false;
		}
	}

	WS_GEN_VECTOR_APPEND_RANGE(
		handlers->handlers,
		builtin_handlers,
		builtin_handlers + (sizeof(builtin_handlers) / sizeof(builtin_handlers[0])),
		success
		);
	return success;
}

bool server_create(
	server_t *s,
	settings_t const *settings,
	unsigned port
	)
{
	if (!socket_create(&s->acceptor))
	{
		fprintf(stderr, "Could not create acceptor\n");
		return false;
	}

	if (!socket_bind(s->acceptor, port))
	{
		fprintf(stderr, "Could not bind acceptor to port %u\n", (unsigned)port);
		socket_destroy(s->acceptor);
		return false;
	}

	node_plugin_manager_create(&s->plugins);
	request_handler_manager_create(&s->request_handlers);

	return true;
}


static void client_thread_proc(void *client_ptr)
{
	client_t * const client = client_ptr;

	client_serve(client);
	client_destroy(client);

	free(client);

	thread_quit();
}

static void handle_client(
	socket_t s,
	const host_entry_t *locations_begin,
	const host_entry_t *locations_end)
{
	thread_t client_thread;
	client_t * const client = malloc(sizeof(*client));

	if (!client)
	{
		socket_destroy(s);
		return;
	}

	client_create(client, s, locations_begin, locations_end);

	if (!thread_create(&client_thread, client_thread_proc, client))
	{
		client_destroy(client);
		free(client);
	}
}

void server_run(server_t *s)
{
	socket_t client;

	while (socket_accept(s->acceptor, &client))
	{
		handle_client(
			client,
			WS_GEN_VECTOR_BEGIN(s->hosts),
			WS_GEN_VECTOR_END(s->hosts)
			);
	}
}

void server_destroy(server_t *s)
{
	host_entry_t *begin = WS_GEN_VECTOR_BEGIN(s->hosts);
	host_entry_t * const end = WS_GEN_VECTOR_END(s->hosts);

	for (; begin != end; ++begin)
	{
		host_entry_destroy(begin);
	}

	WS_GEN_VECTOR_DESTROY(s->hosts);

	socket_destroy(s->acceptor);
	request_handler_manager_destroy(&s->request_handlers);
	node_plugin_manager_destroy(&s->plugins);
}
