#ifndef WS_NODE_PLUGIN_MANAGER_H
#define WS_NODE_PLUGIN_MANAGER_H


#include "common/generic_vector.h"
#include "node_plugin.h"


typedef struct node_plugin_manager_t
{
	WS_GEN_VECTOR(plugins, node_plugin_t);
}
node_plugin_manager_t;

void node_plugin_manager_create(node_plugin_manager_t *m);
void node_plugin_manager_destroy(node_plugin_manager_t *m);


#endif
