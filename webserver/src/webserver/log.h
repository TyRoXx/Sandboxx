#ifndef WS_LOG_H
#define WS_LOG_H


#include "common/config.h"
#include "common/mutex.h"
#include <stdio.h>
#include <stdarg.h>


typedef struct log_t
{
	FILE *out;
	mutex_t out_mutex;
}
log_t;


void log_create(log_t *log, FILE *out);
void log_destroy(log_t *log);
void log_write(log_t *log, char const *format, ...);
void log_writev(log_t *log, char const *format, va_list args);
FILE *log_get_out(log_t *log);
mutex_t *log_get_out_mutex(log_t *log);


#endif
