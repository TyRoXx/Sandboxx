#ifndef WS_FUNCTION_H
#define WS_FUNCTION_H


typedef struct function_t
{
	void (*ptr)(void *);
	void *data;
}
function_t;


void function_create(function_t *this, void (*ptr)(void *), void *data);
void function_call(const function_t *this);


#endif
