#include "online_database.h"

OnlineDatabase::OnlineDatabase(unsigned int id, Socket & socket, const char * ip, unsigned short port, unsigned int bufferSize) : 
	socket(socket),
	ip(ip),
	port(port),
	bufferSize(bufferSize)
{
	this->id = id;
	receiveBuffer = (char *) calloc(bufferSize, sizeof(char));
	tranceiveBuffer = (char *) calloc(bufferSize, sizeof(char));
}

OnlineDatabase::~OnlineDatabase()
{
	free(receiveBuffer);
	free(tranceiveBuffer);
}

void
OnlineDatabase::clearBuffers()
{
	memset(receiveBuffer, 0, bufferSize);
	memset(tranceiveBuffer, 0, bufferSize);
}

void
OnlineDatabase::setName(std::string name)
{
	this->name = name;
}
std::string
OnlineDatabase::getName()
{
	return name;
}

std::string
OnlineDatabase::select(std::string query)
{
	clearBuffers();
	
	socket.connectSocket();
	socket.writeSocket(&query[0] , strlen(&query[0])+1);
	socket.readSocket(receiveBuffer, bufferSize);
//	printf("receiveBuffer: %s\n", receiveBuffer); fflush(stdout);
	socket.disconnectSocket();

	return std::string(receiveBuffer);
}

int
OnlineDatabase::create(std::string query)
{
	return 0;
}
int
OnlineDatabase::insert(std::string query)
{
	clearBuffers();
	
	socket.connectSocket();
	socket.writeSocket(&query[0] , strlen(&query[0])+1);
	socket.readSocket(receiveBuffer, bufferSize);
//	printf("receiveBuffer: %s\n", receiveBuffer); fflush(stdout);
	socket.disconnectSocket();
	
	return atoi(receiveBuffer);
}
int
OnlineDatabase::update(std::string query)
{
	clearBuffers();
	
	socket.connectSocket();
	socket.writeSocket(&query[0] , strlen(&query[0])+1);
	socket.readSocket(receiveBuffer, bufferSize);
//	printf("receiveBuffer: %s\n", receiveBuffer); fflush(stdout);
	socket.disconnectSocket();
	
	return atoi(receiveBuffer);
}
int
OnlineDatabase::drop(std::string query)
{
	return 0;
}
