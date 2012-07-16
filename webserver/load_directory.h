#ifndef WS_LOAD_DIRECTORY_H
#define WS_LOAD_DIRECTORY_H


#include "config.h"


struct directory_t;


bool load_directory(
	struct directory_t *directory,
	const char *begin,
	const char *end
	);


#endif
