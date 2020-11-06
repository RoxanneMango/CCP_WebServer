#ifndef SOCKET_H
#define SOCKET_H

class Socket
{
public:
	int fileDescriptor;

	virtual void connectSocket() = 0;
	virtual void listenSocket(Socket & s) = 0;
	virtual void writeSocket(char * buffer, unsigned int numOfBytes) = 0;
	virtual void readSocket(char * buffer, unsigned int numOfBytes) = 0;
	virtual void disconnectSocket() = 0;
	
	virtual char * getIpAddress() = 0;
};

#endif // SOCKET_H