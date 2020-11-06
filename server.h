#ifndef SERVER_H
#define SERVER_H

#include <vector>

#include <string.h>

#include "socket.h"
#include "database.h"

class Server
{
protected:
	unsigned int bufferSize;
	
	char * receiveBuffer;
	char * tranceiveBuffer;
	char * buffer;

	unsigned int selectedDatabase;
	std::vector<Database *> databases;

	Socket & serverSocket;
	Socket & clientSocket;

	void clearBuffers();
	void resizeBuffers();
public:
	Server(Socket & serverSocket, Socket & clientSocket);
	~Server();

	void setBufferSize(unsigned int bufferSize);

	void addDatabase(Database * database);
	void removeDatabase(unsigned int id);

	void listenClient();
	//
	void readSocket();
	void writeSocket();
	void disconnectSocket();
};

#endif // SERVER_H