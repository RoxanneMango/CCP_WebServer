#include <winsock2.h>
#include <stdio.h>

class Socket
{
protected:
	WSAData wsa;
	SOCKET fileDescriptor;
	struct sockaddr_in winSocket;
	int addressLength;
public:
	Socket(const char * ip, unsigned short port);
	Socket();
	~Socket();
	
	 void connectSocket();
	 void listenSocket(Socket & s);
	 void writeSocket(char * buffer, unsigned int numOfBytes);
	 void readSocket(char * buffer, unsigned int numOfBytes);
	 void disconnectSocket();
	
	 char * getIpAddress();
};