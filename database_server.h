#ifndef DATABASE_SERVER_H
#define DATABASE_SERVER_H

#include <vector>

#include "server.h"
#include "socket.h"

#include <unistd.h>
#include <time.h>

class Column
{
public:
	Column(std::string name) : name(name)
	{}
	~Column()
	{}
	std::string name;
	std::vector<std::string> values;
};

class Table
{
public:
	Table(std::string name) : name(name)
	{}
	~Table()
	{}
	
	std::string name;
	std::vector<Column> columns;
};

class SQL_Database
{
public:
	public:
	SQL_Database(std::string name) : name(name)
	{}
	~SQL_Database()
	{}
	
	std::string name;
	std::vector<Table> tables;
};

class DatabaseServer : public Server
{
private:
	unsigned int id;
	int index = 0;
	std::vector<SQL_Database> databases;

	char * bufferPtr;

	bool isLetter(char c);
	bool isNumber(char c);
	
	bool isString(char * string);
	bool isDate(char * string);
	bool isIp(char * string);
	
public:
	DatabaseServer(unsigned int id, Socket & listener, Socket & client, unsigned int bufferSize);
	~DatabaseServer();

	void processQuery();

	int load(const char * path);
	int save(const char * path);

	std::string select();
	int create();
	int insert();
	int update();
	int drop();
	
	void print();
	void printSelected();
	
	void printBuffer()
	{
		printf("buffer: %s\n", receiveBuffer);
	}
};

#endif // DATABASE_SERVER_H