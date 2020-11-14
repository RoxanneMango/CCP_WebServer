#ifndef SOCKET_H
#define SOCKET_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include "winsock2_listen_socket.h"
#elif defined(__linux__) || (__unix__) || defined(_POSIX_VERSION)
	#include "arpa_listen_socket.h"
#else
	#error "Unknown compiler"
#endif

#endif // SOCKET_H