#include "node_plugin.h"
#include <stdlib.h>


typedef void (*np_get_info)(char *, size_t, unsigned *);

static const size_t PluginNameMaxLength = 32;

bool node_plugin_load(node_plugin_t *plugin, const char *library_file)
{
	np_get_info get_info;

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
	return true;

on_error:
	node_plugin_destroy(plugin);
	return false;
}

void node_plugin_destroy(node_plugin_t *plugin)
{
	free(plugin->name);
	dyn_lib_close(plugin->library);
}
