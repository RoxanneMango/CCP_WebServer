#ifndef ARPA_SOCKET_H
#define ARPA_SOCKET_H

#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

class Socket
{
protected:
	int fileDescriptor;
	struct sockaddr_in arpaSocket;
	socklen_t addressLength;

public:
	Socket(const char * ip, unsigned short port);
	Socket();
	~Socket();
	
	void listenSocket(Socket & s);
	
	void writeSocket(char * buffer, unsigned int numOfBytes);
	void readSocket(char * buffer, unsigned int numOfBytes);
	
	void connectSocket();
	void disconnectSocket();

	char * getIpAddress();
};

#endif // ARPA_SOCKET_H