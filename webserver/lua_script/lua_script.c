#include "lua_script.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "common/load_file.h"
#include "http/directory.h"
#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef struct execution_context_t
{
	const char *url;
	http_response_t *response;
	buffer_t headers;
}
execution_context_t;

static int script_get_url(lua_State *L)
{
	execution_context_t * const execution =
		lua_touserdata(L, lua_upvalueindex(1));
	lua_pushstring(L, execution->url);
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

static int script_add_header(lua_State *L)
{
	execution_context_t * const execution =
		lua_touserdata(L, lua_upvalueindex(1));
	const char *key = lua_tostring(L, -2);
	const char *value = lua_tostring(L, -1);
	http_header_t header;

	if (!key ||
		!value)
	{
		return 0;
	}

	header.key = strdup(key);
	header.value = strdup(value);

	if (!header.key ||
		!header.value ||
		!buffer_append(&execution->headers, &header, sizeof(header)))
	{
		http_header_destroy(&header);
		return 0;
	}

	return 0;
}

static void move_headers(buffer_t *from, http_response_t *to)
{
	to->header_count = (from->size / sizeof(*to->headers));
	to->headers = (http_header_t *)from->data;
	from->data = 0;
	from->size = 0;
	from->capacity = 0;
}

static bool handle_lua_request(
	const char *url,
	directory_entry_t *entry,
	struct http_response_t *response
	)
{
	bool result = false;
	const buffer_t * const script = entry->data;
	execution_context_t execution = {url, response};
	lua_State *L = luaL_newstate();
	if (!L)
	{
		return false;
	}

	buffer_create(&execution.headers);

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_get_url, 1);
	lua_setglobal(L, "GetURL");

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_echo, 1);
	lua_setglobal(L, "Echo");

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_add_header, 1);
	lua_setglobal(L, "AddHeader");

	if (luaL_loadbuffer(L, script->data, script->size, "script") == LUA_OK &&
		lua_pcall(L, 0, LUA_MULTRET, 0) == LUA_OK)
	{
		result = true;
	}
	else
	{
		const char * const error_str = lua_tostring(L, -1);
		if (error_str)
		{
			fprintf(stderr, "Lua error: %s\n", error_str);
			lua_pop(L, 1);
		}
	}

	move_headers(&execution.headers, response);

	lua_close(L);
	buffer_destroy(&execution.headers);
	return result;
}


static void destroy_lua_script(directory_entry_t *entry)
{
	buffer_t * const script = entry->data;
	buffer_destroy(script);
	free(entry->data);
}

bool initialize_lua_script(
	directory_entry_t *entry,
	const char *args
	)
{
	buffer_t *script = malloc(sizeof(*script));
	if (!script)
	{
		return false;
	}
	buffer_create(script);

	if (!load_buffer_from_file_name(script, args))
	{
		buffer_destroy(script);
		free(script);
		return false;
	}

	entry->data = script;
	entry->destroy = destroy_lua_script;
	entry->handle_request = handle_lua_request;
	return true;
}
