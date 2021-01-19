#ifndef ONLINE_DATABASE_H
#define ONLINE_DATABASE_H

#include <vector>
#include <thread>

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

	std::string sendQuery(std::string query);
	std::string save(std::vector<User *> & users);
	std::string save(User & user);
};


#endif // ONLINE_DATABASE_H