#ifndef WS_HTTP_REQUEST_H
#define WS_HTTP_REQUEST_H


#include "common/config.h"
#include <stddef.h>


typedef struct http_request_t
{
	char *method;
	char *url;
	char *host;
}
http_request_t;


bool http_request_parse(
	http_request_t *request,
	int (*read)(void *),
	void *data);
void http_request_destroy(
	http_request_t *request);


#endif
