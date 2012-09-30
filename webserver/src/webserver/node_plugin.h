#ifndef WS_NODE_PLUGIN_H
#define WS_NODE_PLUGIN_H


#include "common/config.h"
#include "dynamic_library.h"


typedef struct node_plugin_t
{
	dyn_lib_t library;
	unsigned api_version;
	char *name;
}
node_plugin_t;


bool node_plugin_load(node_plugin_t *plugin, const char *library_file);
void node_plugin_destroy(node_plugin_t *plugin);


#endif
