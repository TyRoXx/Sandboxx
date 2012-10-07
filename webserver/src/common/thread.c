#include "thread.h"


#ifdef WS_WIN32

#include <process.h>

bool thread_create(thread_t *thread, void (*function)(void *), void *data)
{
	/*_beginthread returns 1L on an error, in which case errno is set to EAGAIN 
	if there are too many threads or to EINVAL if the argument is invalid or the 
	stack size is incorrect.*/
	uintptr_t const rc = _beginthread(function, 0, data);
	if (rc == 1)
	{
		return false;
	}

	*thread = (thread_t)rc;
	return true;
}

void thread_destroy(thread_t thread)
{
	CloseHandle(thread);
}

void thread_join(thread_t thread)
{
	WaitForSingleObject(thread, INFINITE);
}

void thread_quit(void)
{
	_endthread();
}

#else

#include <stdlib.h>

typedef struct function_and_data_t
{
	void (*function)(void *);
	void *data;
}
function_and_data_t;

static void *function_wrapper(void *func_data_ptr)
{
	function_and_data_t const func_data = *(function_and_data_t *)func_data_ptr;
	free(func_data_ptr);
	func_data.function(func_data.data);
	return 0;
}

bool thread_create(thread_t *thread, void (*function)(void *), void *data)
{
	function_and_data_t * const func_data = malloc(sizeof(*func_data));
	if (!func_data)
	{
		return false;
	}

	func_data->function = function;
	func_data->data = data;
	return pthread_create(thread, 0, function_wrapper, func_data) == 0;
}

void thread_destroy(thread_t thread)
{
	thread_join(thread);
}

void thread_join(thread_t thread)
{
	pthread_join(thread, 0);
}

void thread_quit(void)
{
	pthread_exit(0);
}


#endif
