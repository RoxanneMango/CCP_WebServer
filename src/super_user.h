#ifndef SUPER_USER_H
#define SUPER_USER_H

#include "user.h"

class SuperUser : public User
{
private:

public:
	SuperUser(int id, std::string name);
	~SuperUser();
};

#endif // SUPER_USER_H