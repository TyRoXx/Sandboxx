#ifndef WS_STRING_UTIL_H
#define WS_STRING_UTIL_H


#include "config.h"
#include <stddef.h>


char *string_duplicate(char const *original);
char *data_duplicate(char const *data, size_t size);
size_t string_index_of(char const *str, char c);


#endif
