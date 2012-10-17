#ifndef WS_SUB_DIRECTORY_H
#define WS_SUB_DIRECTORY_H


#include "common/config.h"


struct directory_entry_t;
struct loadable_handler_t;


bool initialize_sub_directory(
	struct directory_entry_t *entry,
	char const *args,
	const struct loadable_handler_t *handlers_begin,
	const struct loadable_handler_t *handlers_end,
	char const *current_fs_dir,
	void const *data
	);


#endif
