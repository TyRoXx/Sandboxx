#ifndef WS_SOCKET_H
#define WS_SOCKET_H


#include "config.h"
#include "string.h"
#include <stdint.h>
#include <stddef.h>


#ifdef WS_WIN32
#include <WinSock2.h>
#include <Windows.h>
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif


typedef struct ip_address_t
{
	char digits[4];
}
ip_address_t;

typedef struct socket_address_t
{
	ip_address_t ip;
	uint16_t port;
}
socket_address_t;


bool socket_create(socket_t *socket);
void socket_destroy(socket_t socket);
bool socket_bind(socket_t socket, uint16_t port);
bool socket_accept(socket_t socket, socket_t *accepted, socket_address_t *address /* optional */);
bool socket_receive(socket_t socket, void *data, size_t size, size_t *received);
bool socket_send(socket_t socket, const void *data, size_t size);
void socket_shutdown(socket_t socket);


bool ip_address_to_string(string_t *dest, ip_address_t source);


#endif
