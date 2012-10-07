#ifndef WS_NODE_PLUGIN_H
#define WS_NODE_PLUGIN_H


#include "common/config.h"
#include "dynamic_library.h"
#include <stddef.h>


struct http_request_t;
struct http_response_t;


typedef struct node_plugin_request_handler_t
{
	bool (*function)(
		const char *method,
		const char *url,
		const char *host,
		const char * const *request_headers,
		const char *request_body,
		size_t request_body_size,
		char **response_headers,
		char **response_body,
		void *data
		);
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
bool node_plugin_handle_request(
	node_plugin_t *plugin,
	const struct http_request_t *request,
	struct http_response_t *response);
void node_plugin_destroy(node_plugin_t *plugin);


#endif
