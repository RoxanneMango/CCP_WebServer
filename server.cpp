#include "server.h"

Server::Server(	Socket & serverSocket, Socket & clientSocket ) :
	serverSocket(serverSocket),
	clientSocket(clientSocket)
{	
	// default bufferSize
	this->bufferSize = 4096;
	
	// allocate memory for all the buffers with size bufferSize
	this->receiveBuffer = (char *) calloc(bufferSize, sizeof(char));
	this->tranceiveBuffer = (char *) calloc(bufferSize, sizeof(char));
	this->buffer = (char *) calloc(bufferSize, sizeof(char));

	selectedDatabase = 0;
}

Server::~Server()
{
	// free all allocated memory for buffers
	free(receiveBuffer);
	free(tranceiveBuffer);
	free(buffer);
}

void
Server::setBufferSize(unsigned int bufferSize)
{
	this->bufferSize = bufferSize;
	resizeBuffers();
}

void
Server::resizeBuffers()
{
	this->receiveBuffer = (char *) realloc(receiveBuffer, bufferSize * sizeof(char));
	this->tranceiveBuffer = (char *) realloc(tranceiveBuffer, bufferSize * sizeof(char));
	this->buffer = (char *) realloc(buffer, bufferSize * sizeof(char));
}

void
Server::addDatabase(Database * database)
{
	this->databases.push_back(database);
}
void
Server::removeDatabase(unsigned int id)
{
	for(unsigned int i = 0; i < databases.size(); ++i)
	{
		if(databases[i]->id == id)
		{
			databases.erase(databases.begin() + i);
			break;
		}
	}
}

void
Server::listenClient()
{
	clientSocket.listenSocket(serverSocket);
}

void
Server::readSocket()
{
	clientSocket.readSocket(receiveBuffer, bufferSize);
}

void
Server::writeSocket()
{	
	snprintf(tranceiveBuffer, strlen(buffer) + 1, "%s", buffer);
	clientSocket.writeSocket(tranceiveBuffer, strlen(tranceiveBuffer));
	clearBuffers();
}

void
Server::disconnectSocket()
{
	clientSocket.disconnectSocket();
}

void
Server::clearBuffers()
{
	memset(receiveBuffer, 0, bufferSize);
	memset(tranceiveBuffer, 0, bufferSize);
	memset(buffer, 0, bufferSize);
}
