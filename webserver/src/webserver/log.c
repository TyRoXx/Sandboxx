#include "log.h"


void log_create(log_t *log, FILE *out)
{
	log->out = out;
	mutex_create(&log->out_mutex);
}

void log_destroy(log_t *log)
{
	mutex_destroy(&log->out_mutex);
}

void log_write(log_t *log, char const *format, ...)
{
	va_list args;
	va_start(args, format);

	log_writev(log, format, args);

	va_end(args);
}

void log_writev(log_t *log, char const *format, va_list args)
{
	mutex_lock(&log->out_mutex);

	vfprintf(log->out, format, args);
	fputs("\n", log->out);

	mutex_unlock(&log->out_mutex);
}

FILE *log_get_out(log_t *log)
{
	return log->out;
}

mutex_t *log_get_out_mutex(log_t *log)
{
	return &log->out_mutex;
}
