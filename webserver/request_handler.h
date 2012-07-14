#ifndef WS_REQUEST_HANDLER_H
#define WS_REQUEST_HANDLER_H


#include "config.h"


struct http_request_t;
struct http_response_t;


typedef struct request_handler_t
{
	bool (*function)(const struct http_request_t *, struct http_response_t *, void *);
	void *data;
}
request_handler_t;


#endif
