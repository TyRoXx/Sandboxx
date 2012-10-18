#ifndef WS_NODE_PLUGIN_H
#define WS_NODE_PLUGIN_H


#include "common/config.h"
#include "common/dynamic_library.h"
#include <stddef.h>


struct http_request_t;
struct http_response_t;


typedef struct node_plugin_request_handler_t
{
	bool (*function)(
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
		void const *data
		);
	void (*cleanup)(struct node_plugin_request_handler_t *);
	void const *data;
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


bool node_plugin_load(node_plugin_t *plugin, char const *library_file);
bool node_plugin_handle_request(
	node_plugin_t const *plugin,
	const struct http_request_t *request,
	struct http_response_t *response);
void node_plugin_destroy(node_plugin_t *plugin);


#endif
