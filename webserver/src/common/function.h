#ifndef WS_FUNCTION_H
#define WS_FUNCTION_H


#include "config.h"


typedef struct function_t
{
	void (*ptr)(void *);
	void *data;
}
function_t;


void function_create(function_t *this, void (*ptr)(void *), void *data);
void function_call(const function_t *this);
void function_set_nothing(function_t *this);
bool function_is_set(const function_t *this);


#endif
