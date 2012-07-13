#ifndef WS_HTTP_REQUEST_H
#define WS_HTTP_REQUEST_H


#include "config.h"
#include <stddef.h>


typedef enum http_method_t
{
	method_get,
	method_post,
}
http_method_t;


typedef struct http_header_t
{
	char *key;
	char *value;
}
http_header_t;


typedef struct http_request_t
{
	http_method_t method;
	char *path;
	char *search;
	http_header_t *headers;
	size_t header_count;
}
http_request_t;


bool http_request_parse(
	http_request_t *request,
	int (*read)(void *),
	void *data);
void http_request_destroy(
	http_request_t *request);


#endif
