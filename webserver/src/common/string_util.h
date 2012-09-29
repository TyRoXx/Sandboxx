#ifndef WS_STRING_UTIL_H
#define WS_STRING_UTIL_H


#include "config.h"
#include <stddef.h>


char *string_duplicate(const char *original);
char *data_duplicate(const char *data, size_t size);
size_t string_index_of(const char *str, char c);


#endif
