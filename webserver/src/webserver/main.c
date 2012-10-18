#include "common/socket.h"
#include "common/thread.h"
#include "common/path.h"
#include "common/string_util.h"
#include "common/load_file.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/directory.h"
#include "http/load_directory.h"
#include "lua_script/lua_script.h"
#include "file_system/fs_directory.h"
#include "sub_directory/sub_directory.h"
#include "settings.h"
#include "request_handler_manager.h"
#include "node_plugin.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WEBSERVER_WITH_VLD
#include <vld.h>
#endif


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
	const location_t *locations_begin,
	const location_t *locations_end)
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

static bool load_location(
	location_t *loc,
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

static void destroy_locations(location_t *locations_begin, location_t *locations_end)
{
	for (; locations_begin != locations_end; ++locations_begin)
	{
		location_destroy(locations_begin);
	}
}

typedef struct node_plugin_manager_t
{
	WS_GEN_VECTOR(plugins, node_plugin_t);
}
node_plugin_manager_t;

static void node_plugin_manager_create(node_plugin_manager_t *m)
{
	WS_GEN_VECTOR_CREATE(m->plugins);
}

static void node_plugin_manager_destroy(node_plugin_manager_t *m)
{
	node_plugin_t *begin = WS_GEN_VECTOR_BEGIN(m->plugins);
	node_plugin_t *end = WS_GEN_VECTOR_END(m->plugins);

	for (; begin != end; ++begin)
	{
		node_plugin_destroy(begin);
	}

	WS_GEN_VECTOR_DESTROY(m->plugins);
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

static bool load_request_handler_plugins(
	settings_t const *settings,
	node_plugin_manager_t *plugins,
	request_handler_manager_t *handlers
	)
{
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

	return true;
}

static const loadable_handler_t builtin_handlers[] =
{
	{"lua", initialize_lua_script, 0},
	{"fs", initialize_file_system, 0},
	{"dir", initialize_sub_directory, 0},
};

int main(int argc, char **argv)
{
	const unsigned short acceptor_port = ((argc >= 2) ? (unsigned short)atoi(argv[1]) : 8080);
	char const * const settings_file_name = ((argc >= 3) ? argv[2] : "settings.txt");
	socket_t acceptor, client;
	location_t *locations_begin = 0, *locations_end, *loc;
	settings_t settings;
	buffer_t settings_content;
	host_entry_t *host;
	int result;
	request_handler_manager_t request_handlers;
	node_plugin_manager_t plugins;
	bool success;

	buffer_create(&settings_content);

	if (!load_buffer_from_file_name(&settings_content, settings_file_name))
	{
		fprintf(stderr, "Could not load settings file '%s'\n", settings_file_name);
		buffer_destroy(&settings_content);
		return 1;
	}

	if (!settings_create(&settings, settings_content.data, settings_content.data + settings_content.size))
	{
		buffer_destroy(&settings_content);
		return 1;
	}

	buffer_destroy(&settings_content);

	request_handler_manager_create(&request_handlers);
	node_plugin_manager_create(&plugins);

	WS_GEN_VECTOR_APPEND_RANGE(
		request_handlers.handlers,
		builtin_handlers,
		builtin_handlers + (sizeof(builtin_handlers) / sizeof(builtin_handlers[0])),
		success
		);
	if (!success)
	{
		result = 1;
		goto cleanup_0;
	}

	if (!load_request_handler_plugins(&settings, &plugins, &request_handlers))
	{
		result = 1;
		goto cleanup_0;
	}

	locations_begin = loc = malloc(sizeof(*locations_begin) * WS_GEN_VECTOR_SIZE(settings.hosts));
	locations_end = locations_begin;

	for (host = WS_GEN_VECTOR_BEGIN(settings.hosts);
		host != WS_GEN_VECTOR_END(settings.hosts);
		++host, ++loc, ++locations_end)
	{
		if (!load_location(loc, &request_handlers, host->name, host->destination))
		{
			destroy_locations(locations_begin, locations_end);
			settings_destroy(&settings);
			result = 1;
			goto cleanup_0;
		}
	}

	settings_destroy(&settings);

	if (!socket_create(&acceptor))
	{
		fprintf(stderr, "Could not create acceptor\n");
		result = 1;
		goto cleanup_1;
	}

	if (!socket_bind(acceptor, acceptor_port))
	{
		fprintf(stderr, "Could not bind acceptor to port %u\n", (unsigned)acceptor_port);
		result = 1;
		goto cleanup_2;
	}

	while (socket_accept(acceptor, &client))
	{
		handle_client(
			client,
			locations_begin,
			locations_end);
	}

	result = 0;

cleanup_2:
	socket_destroy(acceptor);

cleanup_1:
	destroy_locations(locations_begin, locations_end);
	free(locations_begin);

cleanup_0:
	request_handler_manager_destroy(&request_handlers);
	node_plugin_manager_destroy(&plugins);
	return result;
}
