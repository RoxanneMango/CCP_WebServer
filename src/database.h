#ifndef DATABASE_H
#define DATABASE_H

#include <string>

class Database
{
public:
	Database()
	{}
	~Database()
	{}
	
	unsigned int id;
	std::string name;

	virtual std::string select(std::string query) = 0;
	//
	virtual int create(std::string query) = 0;
	virtual int insert(std::string query) = 0;
	virtual int update(std::string query) = 0;
	virtual int drop(std::string query) = 0;
};

#endif // DATABASE_H