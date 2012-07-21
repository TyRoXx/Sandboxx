#include "function.h"


void function_create(function_t *this, void (*ptr)(void *), void *data)
{
	this->ptr = ptr;
	this->data = data;
}

void function_call(const function_t *this)
{
	this->ptr(this->data);
}
