#include "socket.h"
#include <stdio.h>


#ifdef WS_UNIX
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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

bool socket_accept(socket_t socket, socket_t *accepted, socket_address_t *address)
{
	typedef
#ifdef WS_WIN32
		int
#else
		socklen_t
#endif
		address_size_t;

	struct sockaddr_in temp_address = {0};
	address_size_t temp_address_size = sizeof(temp_address);

	*accepted = accept(socket, (struct sockaddr *)&temp_address, &temp_address_size);
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

	if (address)
	{
		uint32_t const ip = temp_address.sin_addr.s_addr;
		memcpy(address->ip.digits, &ip, sizeof(address->ip.digits));

		address->port = ntohs(temp_address.sin_port);
	}
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
	char const *remaining = data;
	char const * const end = (remaining + size);

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

void socket_shutdown(socket_t socket)
{
	int const how =
#ifdef _WIN32
		SD_BOTH
#else
		SHUT_RDWR
#endif
		;

	shutdown(socket, how);
}


bool ip_address_to_string(string_t *dest, ip_address_t source)
{
	char buffer[16];
	sprintf(
		buffer,
		"%u.%u.%u.%u",
		source.digits[0],
		source.digits[1],
		source.digits[2],
		source.digits[3]
		);
	
	return string_assign_c_str(dest, buffer);
}
