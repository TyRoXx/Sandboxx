#ifndef WS_SOCKET_H
#define WS_SOCKET_H


#include "config.h"
#include <stdint.h>
#include <stddef.h>


#ifdef WS_WIN32
#include <WinSock2.h>
#include <Windows.h>
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif


bool socket_create(socket_t *socket);
void socket_destroy(socket_t socket);
bool socket_bind(socket_t socket, uint16_t port);
bool socket_accept(socket_t socket, socket_t *accepted);
bool socket_receive(socket_t socket, void *data, size_t size, size_t *received);
bool socket_send(socket_t socket, const void *data, size_t size);
void socket_shutdown(socket_t socket);


#endif
