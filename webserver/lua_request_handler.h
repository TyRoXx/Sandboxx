#ifndef WS_LUA_REQUEST_HANDLER_H
#define WS_LUA_REQUEST_HANDLER_H


#include "config.h"
#include "buffer.h"


struct http_request_t;
struct http_response_t;


typedef struct lua_handler_context_t
{
	buffer_t script;
}
lua_handler_context_t;


bool handle_lua_request(
	const struct http_request_t *request,
	struct http_response_t *response,
	void *data
	);


#endif
