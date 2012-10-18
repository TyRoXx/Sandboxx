#ifndef WS_CONFIG_H
#define WS_CONFIG_H


#ifdef _WIN32
#define WS_WIN32
#else
#define WS_UNIX
#endif


typedef enum
{
	false,
	true
}
bool;


#endif
