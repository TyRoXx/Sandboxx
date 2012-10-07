#include "http_response.h"
#include <string.h>
#include <stdlib.h>


char const *http_status_message(http_status_t status)
{
	switch (status)
	{
	case HttpStatus_Ok: return "OK";
	case HttpStatus_Unauthorized: return "Unauthorized";
	case HttpStatus_Forbidden: return "Forbidden";
	case HttpStatus_NotFound: return "Not Found";
	case HttpStatus_InternalServerError: return "Internal Server Error";
	}
	return "Unknown Status";
}


void http_header_destroy(http_header_t *header)
{
	free(header->key);
	free(header->value);
}

void http_response_destroy(http_response_t *response)
{
	size_t i;

	for (i = 0; i < response->header_count; ++i)
	{
		http_header_destroy(response->headers + i);
	}

	istream_destroy(&response->body);
	function_call(&response->destroy_body);
}

http_header_t *http_response_header(http_response_t *response, char const *key)
{
	size_t i;

	for (i = 0; i < response->header_count; ++i)
	{
		http_header_t * const header = response->headers + i;
		if (!strcmp(header->key, key))
		{
			return header;
		}
	}
	return 0;
}
