#ifndef WS_FS_DIRECTORY_H
#define WS_FS_DIRECTORY_H


#include "common/config.h"


struct directory_entry_t;
struct loadable_handler_t;


bool initialize_file_system(
	struct directory_entry_t *entry,
	const char *args, 
	const struct loadable_handler_t *handlers_begin,
	const struct loadable_handler_t *handlers_end,
	const char *current_fs_dir
	);


#endif
