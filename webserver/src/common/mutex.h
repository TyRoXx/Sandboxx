#ifndef WS_MUTEX_H
#define WS_MUTEX_H


#include "config.h"


#ifdef WS_WIN32
#include <Windows.h>
#else
#endif


typedef struct mutex_t
{
#ifdef WS_WIN32
	CRITICAL_SECTION cs;
#else
	int d;
#endif
}
mutex_t;


void mutex_create(mutex_t *m);
void mutex_destroy(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);


#endif
