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
	char const *path,
	log_t *log
	)
{
	char * const directory_file_name = path_join(path, "directory.txt");
	buffer_t dir_file;

	assert(loc);
	assert(host);
	assert(path);

	buffer_create(&dir_file);

	if (!load_buffer_from_file_name(&dir_file, directory_file_name))
	{
		log_write(log, "Could not load directory file '%s'", directory_file_name);
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
		log_write(log, "Could not parse directory file");
		buffer_destroy(&dir_file);
		return false;
	}

	buffer_destroy(&dir_file);

	loc->host = string_duplicate(host);
	return (loc->host != 0);
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

static bool load_hosts(
	server_t *s,
	settings_t const *settings,
	log_t *log
	)
{
	bool success;
	settings_host_entry_t *settings_begin, *settings_end;
	host_entry_t *host;

	node_plugin_manager_create(&s->plugins);
	request_handler_manager_create(&s->request_handlers);
	WS_GEN_VECTOR_CREATE(s->hosts);

	if (!load_request_handler_plugins(settings, &s->plugins, &s->request_handlers))
	{
		success = false;
		goto leave_0;
	}

	WS_GEN_VECTOR_RESIZE(s->hosts, WS_GEN_VECTOR_SIZE(settings->hosts), success);
	if (!success)
	{
		success = false;
		goto leave_0;
	}

	settings_begin = WS_GEN_VECTOR_BEGIN(settings->hosts);
	settings_end = WS_GEN_VECTOR_END(settings->hosts);
	host = WS_GEN_VECTOR_BEGIN(s->hosts);

	for (; settings_begin != settings_end; ++settings_begin, ++host)
	{
		if (!load_location(host, &s->request_handlers, settings_begin->name, settings_begin->destination, log))
		{
			do
			{
				host_entry_destroy(host);
				--host;
			}
			while (settings_begin != WS_GEN_VECTOR_BEGIN(settings->hosts));

			success = false;
			goto leave_0;
		}
	}

leave_0:
	if (!success)
	{
		WS_GEN_VECTOR_DESTROY(s->hosts);
		request_handler_manager_destroy(&s->request_handlers);
		node_plugin_manager_destroy(&s->plugins);
	}
	return success;
}

bool server_create(
	server_t *s,
	log_t *log,
	settings_t const *settings,
	unsigned port
	)
{
	s->log = log;

	if (!load_hosts(s, settings, log))
	{
		return false;
	}

	if (!socket_create(&s->acceptor))
	{
		log_write(log, "Could not create acceptor");
		return false;
	}

	if (!socket_bind(s->acceptor, port))
	{
		log_write(log, "Could not bind acceptor to port %u", (unsigned)port);
		socket_destroy(s->acceptor);
		return false;
	}

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

static bool make_client_name(
	string_t *name,
	socket_address_t address)
{
	char port_buffer[10];

	sprintf(port_buffer, ":%u", (unsigned)(address.port & 0xffff));

	return
		ip_address_to_string(name, address.ip) &&
		string_append_c_str(name, port_buffer);
}

static void handle_client(
	socket_t s,
	socket_address_t address,
	const host_entry_t *locations_begin,
	const host_entry_t *locations_end,
	log_t *log
	)
{
	thread_t client_thread;
	client_t * const client = malloc(sizeof(*client));
	string_t name;

	if (!client)
	{
		goto leave_0;
	}

	if (!string_create(&name))
	{
		goto leave_0;
	}

	if (!make_client_name(&name, address))
	{
		goto leave_1;
	}

	client_create(
		client,
		s,
		locations_begin,
		locations_end,
		string_data(&name),
		log
		);

	if (thread_create(&client_thread, client_thread_proc, client))
	{
		return;
	}
	else
	{
		goto leave_1;
	}

leave_1:
	string_destroy(&name);

leave_0:
	socket_destroy(s);
	free(client);
}

void server_run(server_t *s)
{
	socket_t client;
	socket_address_t address;

	while (socket_accept(s->acceptor, &client, &address))
	{
		handle_client(
			client,
			address,
			WS_GEN_VECTOR_BEGIN(s->hosts),
			WS_GEN_VECTOR_END(s->hosts),
			s->log
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
