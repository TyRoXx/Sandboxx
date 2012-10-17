#ifndef WS_LOAD_DIRECTORY_H
#define WS_LOAD_DIRECTORY_H


#include "common/config.h"


struct directory_t;
struct directory_entry_t;


typedef struct loadable_handler_t
{
	char *name;
	bool (*initialize)(
		struct directory_entry_t *entry,
		char const *args, 
		const struct loadable_handler_t *,
		const struct loadable_handler_t *,
		char const *current_fs_dir,
		void const *data);
	void const *data;
}
loadable_handler_t;


bool load_directory(
	struct directory_t *directory,
	char const *begin,
	char const *end,
	const loadable_handler_t *handlers_begin,
	const loadable_handler_t *handlers_end,
	char const *current_fs_dir
	);


#endif
