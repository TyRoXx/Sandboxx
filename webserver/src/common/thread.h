#ifndef WS_THREAD_H
#define WS_THREAD_H


#include "config.h"


#ifdef WS_WIN32
#include <Windows.h>
typedef HANDLE thread_t;
#else
#include <pthread.h>
typedef pthread_t thread_t;
#endif


bool thread_create(thread_t *thread, void (*function)(void *), void *data);
void thread_destroy(thread_t thread);
void thread_join(thread_t thread);
void thread_quit(void);


#endif
