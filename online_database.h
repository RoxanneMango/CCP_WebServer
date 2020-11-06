#ifndef ONLINE_DATABASE_H
#define ONLINE_DATABASE_H

#include <string.h>

#include "database.h"
#include "socket.h"

class OnlineDatabase : public Database
{
private:
	unsigned int id;
	std::string name;
	
	Socket & socket;
	
	const char * ip;
	unsigned short port;
	
	unsigned int bufferSize;
	
	char * receiveBuffer;
	char * tranceiveBuffer;
	
	void clearBuffers();
	
public:
	OnlineDatabase(unsigned int id, Socket & socket, const char * ip, unsigned short port, unsigned int bufferSize);
	~OnlineDatabase();

	void setName(std::string name);
	std::string getName();

	std::string select(std::string query);
	//
	int create(std::string query);
	int insert(std::string query);
	int update(std::string query);
	int drop(std::string query);
};


#endif // ONLINE_DATABASE_H