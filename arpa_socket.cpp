#include "arpa_socket.h"

Socket::Socket(const char * ip, unsigned short port)
{
	arpaSocket.sin_family = AF_INET;
	arpaSocket.sin_addr.s_addr = inet_addr(ip);
	arpaSocket.sin_port = htons(port);
}
Socket::Socket()
{}
Socket::~Socket()
{}

void
Socket::listenSocket(Socket & s)
{
	try
	{
		if( (fileDescriptor = accept(s.fileDescriptor, (struct sockaddr *) &arpaSocket, &addressLength ) ) < 0 )
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
Socket::connectSocket()
{
	try
	{
		if ((fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		{ 
			throw "Socket creation error.";
		}
		if(connect(fileDescriptor, (struct sockaddr *)&arpaSocket, sizeof(arpaSocket)) < 0)
		{ 
			throw "Connection Failed.";
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
		if( write(fileDescriptor, buffer, numOfBytes) < 0 )
		{
			throw "Write error.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}

void
Socket::readSocket(char * buffer, unsigned int numOfBytes)
{
	try
	{
		if( read(fileDescriptor, buffer, numOfBytes-1) < 0 )
		{
			throw "Read error.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}

void
Socket::disconnectSocket()
{
	try
	{
		if( close(fileDescriptor) < 0 )
		{
			throw "Could not close socket.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}

char *
Socket::getIpAddress()
{
	return inet_ntoa(arpaSocket.sin_addr);
}