#ifndef ARPA_LISTEN_SOCKET
#define ARPA_LISTEN_SOCKET

#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "arpa_socket.h"

class ArpaListenSocket : public ArpaSocket
{
private:
	const char * ip;
	unsigned short port;

public:
	ArpaListenSocket(const char * ip, unsigned short port);
};

#endif // ARPA_LISTEN_SOCKET