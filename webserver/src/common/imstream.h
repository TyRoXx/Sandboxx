#ifndef WS_IMSTREAM_H
#define WS_IMSTREAM_H


#include "istream.h"
#include "config.h"
#include <stddef.h>


bool imstream_create(
	istream_t *this,
	const void *data,
	size_t size
	);


#endif
