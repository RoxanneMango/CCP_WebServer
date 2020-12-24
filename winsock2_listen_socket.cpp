#include "winsock2_listen_socket.h"

ListenSocket::ListenSocket(const char * ip, unsigned short port) : ip(ip), port(port)
{
	try
	{	
		if( (fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == (unsigned int)SOCKET_ERROR )
		{
			throw "Socket error.";
		}
		memset(&winSocket, 0, sizeof(struct sockaddr_in));
		winSocket.sin_family = AF_INET;
		winSocket.sin_addr.s_addr = inet_addr(ip);
		winSocket.sin_port = htons(port);
		
		if( (bind(fileDescriptor, (struct sockaddr *) &winSocket, sizeof(winSocket))) == SOCKET_ERROR )
		{
			winSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
			winSocket.sin_port = htons(port);
			
			if( (bind(fileDescriptor, (struct sockaddr *) &winSocket, sizeof(winSocket))) == SOCKET_ERROR )
			{
				throw "Bind error.";
			}
		}
		if( (listen(fileDescriptor, 10)) == SOCKET_ERROR )
		{
			throw "Listen error.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s (%d)\n", exception, WSAGetLastError());
	}
}

ListenSocket::~ListenSocket()
{}