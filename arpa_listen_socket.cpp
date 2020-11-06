#include "arpa_listen_socket.h"

ArpaListenSocket::ArpaListenSocket(const char * ip, unsigned short port)
{
	this->ip = ip;
	this->port = port;
	
	try
	{	
		if( (fileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		{
			throw "Socket error.";
		}
		memset(&arpaSocket, 0, sizeof(struct sockaddr_in));
		arpaSocket.sin_family = AF_INET;
		arpaSocket.sin_addr.s_addr = inet_addr(ip);
		arpaSocket.sin_port = htons(port);
		
		if( (bind(fileDescriptor, (struct sockaddr *) &arpaSocket, sizeof(arpaSocket))) < 0 )
		{
			arpaSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
			arpaSocket.sin_port = htons(port);
			
			if( (bind(fileDescriptor, (struct sockaddr *) &arpaSocket, sizeof(arpaSocket))) < 0 )
			{
				throw "Bind error.";
			}
		}
		if( (listen(fileDescriptor, 10)) < 0 )
		{
			throw "Listen error.";
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}