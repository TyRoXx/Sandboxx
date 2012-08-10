#include "function.h"


void function_create(function_t *this, void (*ptr)(void *), void *data)
{
	this->ptr = ptr;
	this->data = data;
}

void function_call(const function_t *this)
{
	if (this->ptr)
	{
		this->ptr(this->data);
	}
}

static void do_nothing(void *data)
{
}

void function_set_nothing(function_t *this)
{
	this->ptr = do_nothing;
	this->data = 0;
}

bool function_is_set(const function_t *this)
{
	return (this->ptr != 0);
}
