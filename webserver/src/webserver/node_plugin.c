#include "node_plugin.h"
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

typedef bool (*np_handle_request_v0)(const char *, const char * const *, char **);

static bool load_request_handler_v0(node_plugin_t *plugin)
{
	node_plugin_request_handler_t *handler = &plugin->request_handler;

	const char * const function_name = "np_handle_request";
	np_handle_request_v0 function = dyn_lib_find(plugin->library, function_name);
	if (!function)
	{
		fprintf(stderr, "Plugin %s: Could not find function %s\n",
			plugin->name,
			function_name);
		return false;
	}

	handler->function = function;
	handler->data = 0;
	handler->cleanup = 0;
	return true;
}

static bool load_request_handler(node_plugin_t *plugin)
{
	switch (plugin->api_version)
	{
	case 0:
		return load_request_handler_v0(plugin);

	default:
		fprintf(stderr, "Plugin %s: Unknown API version %u\n",
			plugin->name,
			(unsigned)plugin->api_version);
		return false;
	}
}

bool node_plugin_load(node_plugin_t *plugin, const char *library_file)
{
	np_get_info get_info;

	plugin->request_handler.cleanup = 0;

	plugin->library = dyn_lib_open(library_file);
	if (!plugin->library)
	{
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

void node_plugin_destroy(node_plugin_t *plugin)
{
	free(plugin->name);
	dyn_lib_close(plugin->library);
	node_plugin_request_handler_destroy(&plugin->request_handler);
}
