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


void http_response_destroy(http_response_t *response)
{
	string_destroy(&response->headers);
	istream_destroy(&response->body);
	function_call(&response->destroy_body);
}
