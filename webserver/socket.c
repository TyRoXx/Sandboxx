#include "socket.h"

#ifdef WS_UNIX
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif


static const socket_t InvalidSocket =
#ifdef WS_WIN32
	INVALID_SOCKET
#else
	-1
#endif
	;

#ifdef WS_WIN32
static bool wsa_increment()
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
}

static void wsa_decrement()
{
	WSACleanup();
}
#endif

bool socket_create(socket_t *socket_)
{
#ifdef WS_WIN32
	if (!wsa_increment())
	{
		return false;
	}
#endif

	*socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*socket_ == InvalidSocket)
	{
#ifdef WS_WIN32
		wsa_decrement();
#endif
		return false;
	}

	return true;
}

void socket_destroy(socket_t socket)
{
#ifdef WS_WIN32
	closesocket(socket);
	wsa_decrement();
#else
	close(socket);
#endif
}

bool socket_bind(socket_t socket, uint16_t port)
{
	struct sockaddr_in address = {0};
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	return
		(bind(socket, (struct sockaddr *)&address, sizeof(address)) == 0) &&
		(listen(socket, 10) == 0);
}

bool socket_accept(socket_t socket, socket_t *accepted)
{
	*accepted = accept(socket, 0, 0);
	if (*accepted == InvalidSocket)
	{
		return false;
	}

#ifdef WS_WIN32
	if (!wsa_increment())
	{
		return false;
	}
#endif

	return true;
}

bool socket_receive(socket_t socket, void *data, size_t size, size_t *received)
{
	const int rc = recv(socket, data, size, 0);
	if (rc <= 0)
	{
		return false;
	}

	*received = rc;
	return true;
}

bool socket_send(socket_t socket, const void *data, size_t size)
{
	const char *remaining = data;
	const char * const end = (remaining + size);
	while (remaining < end)
	{
		const int rc = send(socket, remaining, (end - remaining), 0);
		if (rc <= 0)
		{
			return false;
		}

		remaining += rc;
	}
	return true;
}
