#include "thread.h"


#ifdef WS_WIN32

#include <process.h>

bool thread_create(thread_t *thread, void (*function)(void *), void *data)
{
	/*_beginthread returns 1L on an error, in which case errno is set to EAGAIN 
	if there are too many threads or to EINVAL if the argument is invalid or the 
	stack size is incorrect.*/
	const uintptr_t rc = _beginthread(function, 0, data);
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

#endif
