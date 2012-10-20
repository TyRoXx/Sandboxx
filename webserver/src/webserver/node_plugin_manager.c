#include "node_plugin_manager.h"


void node_plugin_manager_create(node_plugin_manager_t *m)
{
	WS_GEN_VECTOR_CREATE(m->plugins);
}

void node_plugin_manager_destroy(node_plugin_manager_t *m)
{
	node_plugin_t *begin = WS_GEN_VECTOR_BEGIN(m->plugins);
	node_plugin_t *end = WS_GEN_VECTOR_END(m->plugins);

	for (; begin != end; ++begin)
	{
		node_plugin_destroy(begin);
	}

	WS_GEN_VECTOR_DESTROY(m->plugins);
}
