#ifndef WS_NODE_PLUGIN_H
#define WS_NODE_PLUGIN_H


#include "common/config.h"
#include "dynamic_library.h"
#include <stddef.h>


typedef struct node_plugin_request_handler_t
{
	bool (*function)(const char *, const char * const *, char **);
	void (*cleanup)(struct node_plugin_request_handler_t *);
	void *data;
}
node_plugin_request_handler_t;


void node_plugin_request_handler_destroy(node_plugin_request_handler_t *r);


typedef struct node_plugin_t
{
	dyn_lib_t library;
	unsigned api_version;
	char *name;
	void (*realloc)(void *, size_t);
	void (*free)(void *);
	node_plugin_request_handler_t request_handler;
}
node_plugin_t;


bool node_plugin_load(node_plugin_t *plugin, const char *library_file);
void node_plugin_destroy(node_plugin_t *plugin);


#endif
