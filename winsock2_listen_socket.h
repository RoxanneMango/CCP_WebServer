#include "winsock2_socket.h"

class ListenSocket : public Socket
{
private:
	const char * ip;
	unsigned short port;
public:
	ListenSocket(const char * ip, unsigned short port);
	~ListenSocket();
};