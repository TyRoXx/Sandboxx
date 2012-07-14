#include "lua_request_handler.h"
#include "http_request.h"
#include "http_response.h"
#include <lua.h>
#include <lauxlib.h>
#include <string.h>
#include <stdio.h>


typedef struct execution_context_t
{
	const http_request_t *request;
	http_response_t *response;
}
execution_context_t;

static int script_get_url(lua_State *L)
{
	execution_context_t * const execution =
		lua_touserdata(L, lua_upvalueindex(1));
	lua_pushstring(L, execution->request->url);
	return 1;
}

static int script_echo(lua_State *L)
{
	execution_context_t * const execution =
		lua_touserdata(L, lua_upvalueindex(1));
	const char *text = lua_tostring(L, -1);

	if (!text)
	{
		fprintf(stderr, "Echo() must be called with a string\n");
		return 0;
	}

	if (!buffer_append(
		&execution->response->body,
		text,
		strlen(text)))
	{
		fprintf(stderr, "Echo() failed\n");
	}

	return 0;
}

bool handle_lua_request(
	const struct http_request_t *request,
	struct http_response_t *response,
	void *data
	)
{
	bool result = false;
	const lua_handler_context_t * const handler = (lua_handler_context_t *)data;
	execution_context_t execution = {request, response};
	lua_State *L = luaL_newstate();
	if (!L)
	{
		return false;
	}

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_get_url, 1);
	lua_setglobal(L, "GetURL");

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_echo, 1);
	lua_setglobal(L, "Echo");

	if (luaL_loadbuffer(L, handler->script.data, handler->script.size, "script") == LUA_OK &&
		lua_pcall(L, 0, LUA_MULTRET, 0) == LUA_OK)
	{
		result = true;
	}

	lua_close(L);
	return result;
}

