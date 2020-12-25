#include "user.h"

User::User(int id, std::string name) : id(id), name(name)
{
	currency = "Dollar";
	balance = 0.00;
}

User::User(int id, std::string name, std::string username, std::string password, bool superUser) : 
	id(id), name(name), username(username), password(password), superUser(superUser)
{
	currency = "Dollar";
	balance = 1000.00;
	isReady = true;
}

User::~User()
{}

double
User::getBalance()
{
	return balance;
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

void
User::setBalance(double balance)
{
	this->balance = balance;
}

void
User::addBalance(double balance)
{
	this->balance += balance;
}
void
User::subtractBalance(double balance)
{
	this->balance -= balance;
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