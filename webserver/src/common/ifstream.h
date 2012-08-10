#ifndef WS_IFSTREAM_H
#define WS_IFSTREAM_H


#include "istream.h"
#include "config.h"
#include <stddef.h>
#include <stdio.h>


bool ifstream_create(
	istream_t *this,
	FILE *file
	);


#endif
