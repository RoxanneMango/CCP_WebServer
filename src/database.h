#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include "user.h"

class Database
{
public:
	Database()
	{}
	~Database()
	{}
	
	unsigned int id;
	std::string name;
	bool isBusy = false;

	virtual std::string sendQuery(std::string query) = 0;
	virtual std::string save(std::vector<User *> & users) = 0;
	virtual std::string save(User & user) = 0;
};

#endif // DATABASE_H