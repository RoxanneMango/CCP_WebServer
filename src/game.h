#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

#include "user.h"
#include "param.h"

// Interface class
class Game
{
public:
	int id;
	std::string name;

	unsigned int minUsers;
	unsigned int currentUsers;
	unsigned int maxUsers;
	
	std::vector<void *> users;

	Game(int id) : id(id)
	{}

	virtual User * addUser(User * user)
	{
		users.push_back(user);
		return user;
	}

	virtual std::string input(Param param) = 0;
	
	virtual void init() = 0;
	virtual void run() = 0;
	virtual void stop() = 0;
	
	virtual bool isRunning() = 0;
};

#endif // GAME_H