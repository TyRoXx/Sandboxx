#ifndef WS_DIRECTORY_H
#define WS_DIRECTORY_H


#include "common/config.h"
#include <stddef.h>


struct http_response_t;


typedef struct directory_entry_t
{
	char *name;
	void *data;
	void (*destroy)(struct directory_entry_t *);
	bool (*handle_request)(char const *, struct directory_entry_t *, struct http_response_t *);
}
directory_entry_t;


void directory_entry_destroy(directory_entry_t *entry);


typedef struct directory_t
{
	directory_entry_t *entries;
	size_t entry_count;
	directory_entry_t *default_;
}
directory_t;


void directory_create(directory_t *directory);
void directory_destroy(directory_t *directory);
bool directory_handle_request(const directory_t *directory, char const *path, struct http_response_t *response);


#endif
