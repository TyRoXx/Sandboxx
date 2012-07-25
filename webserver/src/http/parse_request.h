#ifndef WS_HTTP_PARSE_REQUEST_H
#define WS_HTTP_PARSE_REQUEST_H


#include "http_request.h"
#include "common/istream.h"


bool parse_request(
	http_request_t *request,
	istream_t *source
	);


#endif
