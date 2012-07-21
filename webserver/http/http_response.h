#ifndef WS_HTTP_RESPONSE_H
#define WS_HTTP_RESPONSE_H


#include "common/buffer.h"
#include "common/istream.h"
#include "common/function.h"


typedef enum http_status_t
{
	HttpStatus_Ok = 200,
	HttpStatus_Unauthorized = 401,
	HttpStatus_Forbidden = 403,
	HttpStatus_NotFound = 404,
	HttpStatus_InternalServerError = 500,
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
	istream_t body;
	function_t destroy_body;
	size_t body_size;
}
http_response_t;


void http_response_destroy(http_response_t *response);
http_header_t *http_response_header(http_response_t *response, const char *key);


#endif
