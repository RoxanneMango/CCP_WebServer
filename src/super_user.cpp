#include "super_user.h"

SuperUser::SuperUser(int id, std::string) : User(id, name)
{
	superUser = true;
}

SuperUser::~SuperUser()
{}
