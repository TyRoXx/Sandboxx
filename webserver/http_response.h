#ifndef WS_HTTP_RESPONSE_H
#define WS_HTTP_RESPONSE_H


#include "buffer.h"


typedef enum http_status_t
{
	Status_Ok = 200,
	Status_NotFound = 404,
}
http_status_t;


const char *http_status_message(http_status_t status);


typedef struct http_header_t
{
	char *key;
	char *value;
}
http_header_t;


void http_header_destroy(http_header_t *header);


typedef struct http_response_t
{
	http_status_t status;
	http_header_t *headers;
	size_t header_count;
	buffer_t body;
}
http_response_t;


void http_response_destroy(http_response_t *response);
http_header_t *http_response_header(http_response_t *response, const char *key);


#endif
