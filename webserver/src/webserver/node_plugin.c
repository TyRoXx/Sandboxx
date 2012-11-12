#include "node_plugin.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "common/imstream.h"
#include <stdlib.h>
#include <stdio.h>


void node_plugin_request_handler_destroy(node_plugin_request_handler_t *r)
{
	if (r->cleanup)
	{
		r->cleanup(r);
	}
}


typedef void (*np_get_info)(char *, size_t, unsigned *);

static const size_t PluginNameMaxLength = 32;

static bool load_memory_functions(node_plugin_t *plugin)
{
	plugin->realloc = dyn_lib_find(plugin->library, "np_realloc");
	plugin->free    = dyn_lib_find(plugin->library, "np_free");

	if (plugin->realloc &&
		plugin->free)
	{
		return true;
	}

	fprintf(
		stderr,
		"Plugin %s: np_realloc or np_free missing\n",
		plugin->name);
	return false;
}

typedef int (*np_request_handler_fn)(
	char const *method,
	char const *url,
	char const *host,
	char const * const *request_headers,
	char const *request_body,
	size_t request_body_size,
	unsigned *response_status,
	char **response_headers,
	char **response_body,
	size_t *response_body_size);

static bool request_handle_v0_call(
	char const *method,
	char const *url,
	char const *host,
	char const * const *request_headers,
	char const *request_body,
	size_t request_body_size,
	unsigned *response_status,
	char **response_headers,
	char **response_body,
	size_t *response_body_size,
	void const *data)
{
	np_request_handler_fn const function = (void *)data;
	return function(
		method,
		url,
		host,
		request_headers,
		request_body,
		request_body_size,
		response_status,
		response_headers,
		response_body,
		response_body_size);
}

static bool load_request_handler_v0(node_plugin_t *plugin)
{
	node_plugin_request_handler_t *handler = &plugin->request_handler;

	char const * const function_name = "np_handle_request";
	np_request_handler_fn const function = dyn_lib_find(plugin->library, function_name);
	if (!function)
	{
		char const * const name = plugin->name;
		fprintf(stderr,
			"Plugin %s: Could not find function %s\n",
			name,
			function_name);
		return false;
	}

	handler->function = request_handle_v0_call;
	handler->data = function;
	handler->cleanup = 0;
	return true;
}

static bool load_request_handler(node_plugin_t *plugin)
{
	if (!load_memory_functions(plugin))
	{
		return false;
	}

	switch (plugin->api_version)
	{
	case 0:
		return load_request_handler_v0(plugin);

	default:
		{
			char const * const name = plugin->name;
			fprintf(stderr,
				"Plugin %s: Unknown API version %u\n",
				name,
				(unsigned)plugin->api_version);
			return false;
		}
	}
}

bool node_plugin_load(node_plugin_t *plugin, char const *library_file)
{
	np_get_info get_info;

	plugin->request_handler.cleanup = 0;

	plugin->library = dyn_lib_open(library_file);
	if (!plugin->library)
	{
		fprintf(stderr,
			"Could not open shared library %s\n",
			library_file);
		return false;
	}

	plugin->name = malloc(PluginNameMaxLength + 1);
	if (!plugin->name)
	{
		goto on_error;
	}

	get_info = dyn_lib_find(plugin->library, "np_get_info");
	if (!get_info)
	{
		goto on_error;
	}

	plugin->api_version = 0;
	get_info(plugin->name, PluginNameMaxLength, &plugin->api_version);

	if (load_request_handler(plugin))
	{
		return true;
	}

on_error:
	node_plugin_destroy(plugin);
	return false;
}

bool node_plugin_handle_request(
	node_plugin_t const *plugin,
	const struct http_request_t *request,
	struct http_response_t *response)
{
	//currently no headers
	char const * const null_ptr = 0;
	char const * const * const request_headers = &null_ptr;

	//API expects these to be null
	unsigned response_status = 200;
	char *response_headers = 0;
	char *response_body = 0;
	size_t response_body_size = 0;

	if (plugin->request_handler.function(
		request->method,
		request->url,
		request->host,
		request_headers,
		0,
		0,
		&response_status,
		&response_headers,
		&response_body,
		&response_body_size,
		plugin->request_handler.data
		))
	{
		if (!imstream_create(&response->body, response_body, response_body_size))
		{
			plugin->free(response_headers);
			plugin->free(response_body);
			return false;
		}

		response->body_size = response_body_size;
		function_create(&response->destroy_body, plugin->free, response_body);

		response->status = (http_status_t)response_status;

		if (response_headers &&
			!string_assign_c_str(&response->headers, response_headers))
		{
			plugin->free(response_headers);
			return false;
		}

		plugin->free(response_headers);
		return true;
	}

	return false;
}

void node_plugin_destroy(node_plugin_t *plugin)
{
	free(plugin->name);
	dyn_lib_close(plugin->library);
	node_plugin_request_handler_destroy(&plugin->request_handler);
}
