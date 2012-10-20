#include "mutex.h"


#ifdef WS_WIN32

void mutex_create(mutex_t *m)
{
	InitializeCriticalSection(&m->cs);
}

void mutex_destroy(mutex_t *m)
{
	DeleteCriticalSection(&m->cs);
}

void mutex_lock(mutex_t *m)
{
	EnterCriticalSection(&m->cs);
}

void mutex_unlock(mutex_t *m)
{
	LeaveCriticalSection(&m->cs);
}

#else

void mutex_create(mutex_t *m)
{
}

void mutex_destroy(mutex_t *m)
{
}

void mutex_lock(mutex_t *m)
{
}

void mutex_unlock(mutex_t *m)
{
}

#endif
