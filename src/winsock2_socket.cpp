#include "winsock2_socket.h"

Socket::Socket()
{
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
	{
		printf("Failed to initialise winsock: Error (%d)\n", WSAGetLastError());
		this->~Socket();
	}
}

Socket::Socket(const char * ip, unsigned short port)
{
	winSocket.sin_family = AF_INET;
	winSocket.sin_addr.s_addr = inet_addr(ip);
	winSocket.sin_port = htons(port);
	
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
	{
		printf("Failed to initialise winsock: Error (%d)\n", WSAGetLastError());
		this->~Socket();
	}
}


Socket::~Socket()
{
	WSACleanup();
}

void
Socket::connectSocket()
{
	try
	{
		if((fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		{ 
			throw "Socket creation error.";
		}
		if(connect(fileDescriptor, (struct sockaddr *)&winSocket, sizeof(winSocket)) < 0)
		{ 
			throw "Connection Failed.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n (%d)", exception, WSAGetLastError());
	}
}

void
Socket::listenSocket(Socket & s)
{
	try
	{
		if( (fileDescriptor = accept(s.fileDescriptor, (struct sockaddr *) &winSocket, &addressLength ), 0 ) < 0 )
		{
			throw "Listen error.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}

void
Socket::writeSocket(char * buffer, unsigned int numOfBytes)
{
	try
	{
		if( (send(fileDescriptor, buffer, numOfBytes, 0)) < 0 )
		{
			throw "Write error.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s (%d)\n", exception, WSAGetLastError());
	}
}

void
Socket::readSocket(char * buffer, unsigned int numOfBytes)
{
	try
	{
		if( (recv(fileDescriptor, buffer, numOfBytes-1, 0)) == SOCKET_ERROR )
		{
			throw "Read error.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s (%d)\n", exception, WSAGetLastError());
	}
}

void
Socket::disconnectSocket()
{
	closesocket(fileDescriptor);
}

char * 
Socket::getIpAddress()
{
	return inet_ntoa(winSocket.sin_addr);
}