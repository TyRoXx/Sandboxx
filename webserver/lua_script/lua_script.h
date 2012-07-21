#ifndef WS_LUA_SCRIPT_H
#define WS_LUA_SCRIPT_H


#include "common/config.h"
#include "common/buffer.h"


struct directory_entry_t;


bool initialize_lua_script(
	struct directory_entry_t *entry,
	const char *args
	);


#endif
