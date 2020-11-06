#include "user.h"

User::User(int id, std::string name) : id(id), name(name)
{
	currency = "Dollar";
	balance = 0.00;
}

User::User(int id, std::string name, std::string username, std::string password, bool superUser) : id(id), name(name), username(username), password(password), superUser(superUser)
{
	currency = "Dollar";
	balance = 0.00;
}

User::~User()
{}

void
User::addChips(User & user, Chip chip)
{
	user.chips.push_back(chip);
}

void
User::removeChips(User & user, Chip chip)
{
	for(unsigned int i = 0; i < user.chips.size(); ++i)
	{
		if(user.chips[i] == chip)
		{
			user.chips.erase(chips.begin()+i);
			return;
		}
	}
}

double
User::getBalance()
{
	return balance;
}

void
User::setBalance(double balance)
{
	this->balance = balance;
}

std::string
User::getCurrency()
{
	return currency;
}

void
User::setCurrency(std::string currency)
{
	this->currency = currency;
}

void
User::login(std::string token, std::string ip)
{
	loggedIn = true;
	this->token = token;
	this->ip = ip;
	
	printf("User logged in with\n");
	printf("\ttoken {%s};\n", &token[0]);
	printf("\tusername {%s};\n", &username[0]);
	printf("\tip {%s};\n", &ip[0]);
	
}

void
User::logout()
{
	loggedIn = false;
	
	printf("User logged out with\n");
	printf("\ttoken {%s};\n", &token[0]);
	printf("\tip {%s};\n", &ip[0]);
	
}