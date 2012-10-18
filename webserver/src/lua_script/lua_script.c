#include "lua_script.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "common/imstream.h"
#include "common/string_util.h"
#include "common/load_file.h"
#include "common/path.h"
#include "http/directory.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>


typedef struct execution_context_t
{
	char const *url;
	http_response_t *response;
	buffer_t body;
}
execution_context_t;

static int script_get_url(lua_State *L)
{
	execution_context_t * const execution =
		lua_touserdata(L, lua_upvalueindex(1));
	lua_pushstring(L, execution->url);
	return 1;
}

static bool write_escaped(buffer_t *dest, char c)
{
	char *str;
	int rc;

	/*the extra byte is for sprintf's null termination*/
	if (!buffer_reserve(dest, dest->size + 6 + 1))
	{
		return false;
	}

	str = dest->data + dest->size;

	/*sprintf for simplicity*/
	rc = sprintf(str, "&#%u;", (unsigned)(unsigned char)c);

	/*sprintf can fail*/
	if (rc < 0)
	{
		return false;
	}

	dest->size += rc;
	return true;
}

static int script_write(lua_State *L)
{
	execution_context_t * const execution =
		lua_touserdata(L, lua_upvalueindex(1));
	buffer_t * const body = &execution->body;
	char const *text = lua_tostring(L, -1);

	if (!text)
	{
		fprintf(stderr, "Write() must be called with a string\n");
		return 0;
	}

	for (; *text != '\0'; ++text)
	{
		switch (*text)
		{
		case '<':
		case '>':
		case '&':
			if (!write_escaped(body, *text))
			{
				/*silence for now*/
				return 0;
			}
			break;

		default:
			if (!buffer_push_back(body, *text))
			{
				/*silence for now*/
				return 0;
			}
		}
	}

	return 0;
}

static int script_raw(lua_State *L)
{
	execution_context_t * const execution =
		lua_touserdata(L, lua_upvalueindex(1));
	char const * const text = lua_tostring(L, -1);

	if (!text)
	{
		fprintf(stderr, "Raw() must be called with a string\n");
		return 0;
	}

	if (!buffer_append(
		&execution->body,
		text,
		strlen(text)))
	{
		/*nothing for now*/
	}

	return 0;
}

static int script_add_header(lua_State *L)
{
	execution_context_t * const execution =
		lua_touserdata(L, lua_upvalueindex(1));
	char const * const key = lua_tostring(L, -2);
	char const * const value = lua_tostring(L, -1);
	string_t * const headers = &execution->response->headers;
	
	int const result =
		key &&
		value &&
		string_append_c_str(headers, key) &&
		string_append_c_str(headers, ": ") &&
		string_append_c_str(headers, value) &&
		string_append_c_str(headers, "\r\n");

	lua_pushboolean(L, result);
	return 1;
}

static bool handle_lua_request(
	char const *url,
	directory_entry_t *entry,
	struct http_response_t *response
	)
{
	bool result = false;
	char const * const script_path = entry->data;
	buffer_t script;
	execution_context_t execution = {url, response};
	lua_State * const L = luaL_newstate();

	if (!L)
	{
		return false;
	}

	buffer_create(&script);

	if (!load_buffer_from_file_name(&script, script_path))
	{
		lua_close(L);
		buffer_destroy(&script);
		return false;
	}

	luaopen_string(L);
	luaopen_base(L);
	luaopen_math(L);

	buffer_create(&execution.body);

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_get_url, 1);
	lua_setglobal(L, "GetURL");

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_write, 1);
	lua_setglobal(L, "Write");

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_raw, 1);
	lua_setglobal(L, "Raw");

	lua_pushlightuserdata(L, &execution);
	lua_pushcclosure(L, script_add_header, 1);
	lua_setglobal(L, "AddHeader");

	if (luaL_loadbuffer(L, script.data, script.size, "script") == LUA_OK &&
		lua_pcall(L, 0, LUA_MULTRET, 0) == LUA_OK)
	{
		void * const body_data = execution.body.data;
		const size_t body_size = execution.body.size;

		execution.body.size = execution.body.capacity = 0;
		execution.body.data = 0;

		if (imstream_create(&response->body, body_data, body_size))
		{
			function_create(&response->destroy_body, free, body_data);
			response->body_size = body_size;
			result = true;
		}
		else
		{
			free(body_data);
		}
	}
	else
	{
		static char const Message[] = "Internal server error";

		char const * const error_str = lua_tostring(L, -1);
		if (error_str)
		{
			fprintf(stderr, "Lua error: %s\n", error_str);
			lua_pop(L, 1);
		}

		response->status = HttpStatus_InternalServerError;
		response->body_size = strlen(Message);

		if (!imstream_create(&response->body, Message, response->body_size))
		{
			fprintf(stderr, "Error in error handling. Terminating to prevent undefined behaviour\n");
			exit(1);
		}

		result = true;
		goto return_;
	}
	
return_:
	lua_close(L);
	buffer_destroy(&script);
	return result;
}


static void destroy_lua_script(directory_entry_t *entry)
{
	free(entry->data);
}

bool initialize_lua_script(
	struct directory_entry_t *entry,
	char const *args,
	const struct loadable_handler_t *handlers_begin,
	const struct loadable_handler_t *handlers_end,
	char const *current_fs_dir,
	void const *data
	)
{
	entry->data = path_join(current_fs_dir, args);
	if (!entry->data)
	{
		return false;
	}

	entry->destroy = destroy_lua_script;
	entry->handle_request = handle_lua_request;
	return true;
}
