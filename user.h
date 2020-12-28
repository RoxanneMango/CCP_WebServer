#ifndef USER_H
#define USER_H

#include <vector>
#include <string>

class User
{
protected:
	int id;
	
	std::string ip;
	unsigned short port;
	
	std::string name;
	//
	std::string token;
	std::string username;
	std::string password;
	//
	bool loggedIn = false;

	std::string currency;
	double balance;
	
	time_t timeOfRegistration;
	time_t lastLogin;
	
	bool superUser = false;
public:
	User(int id, std::string name);
	User(int id, std::string name, std::string username, std::string password, bool superUser = false);
	virtual ~User();

	double getBalance();
	void setBalance(double balance);
	void addBalance(double balance);
	void subtractBalance(double balance);
	
	std::string getCurrency();
	void setCurrency(std::string currency);
	
	bool isReady;
	
	void login(std::string token, std::string ip);
	void logout();
	
	std::string getIp() { return ip; }
	unsigned short getPort() { return port; }
	
	std::string getToken() { return token; }
	std::string getUsername() { return username; }
	std::string getPassword() { return password; }
	//
	bool isLoggedIn() { return loggedIn; }
	
	std::string getName() { return name; }
	
	void sudo() { superUser = true; }
	int getID() { return id; }
};

#endif // PLAYER_H