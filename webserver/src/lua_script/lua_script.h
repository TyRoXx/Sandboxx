#ifndef WS_LUA_SCRIPT_H
#define WS_LUA_SCRIPT_H


#include "common/config.h"
#include "common/buffer.h"


struct directory_entry_t;
struct loadable_handler_t;


bool initialize_lua_script(
	struct directory_entry_t *entry,
	char const *args,
	const struct loadable_handler_t *handlers_begin,
	const struct loadable_handler_t *handlers_end,
	char const *current_fs_dir,
	void const *data
	);


#endif
