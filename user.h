#ifndef USER_H
#define USER_H

#include <vector>

#include "chip.h"
#include "card.h"

class User
{
private:
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
	~User();

	// todo ?: create BlackJackPlayer : public User class
	// give it std::vector<std::string> hand attribute
	// give it std::vector<Chip> chips attribute
	
	// [ user_id { card_value , card_suit ; card_value , card_suit , etc , etc } { optional_second_hand } ; ]
	// if player only has one hand, terminate message with a semi-colon ;
	// otherwise look at second hand, then terminate message with a semi-colon ;
	// dealer has user_id of -1
	// dealer cannot have a second hand
//	std::vector<std::string> hand;

	// a user can potentially have 2 hands
	std::vector<Card> hand[2];
	std::vector<Chip> chips;
//	std::vector<Chip> bettingAmount;
	double bettingAmount;
	
	void addChips(User & user, Chip chip);
	void removeChips(User & user, Chip chip);
	
	double getBalance();
	void setBalance(double balance);
	
	std::string getCurrency();
	void setCurrency(std::string currency);
	
	double getBettingAmount()
	{
		/*
		double amount = 0.0;
		for(unsigned int i = 0; i < bettingAmount.size(); ++i)
		{
			amount += bettingAmount[i].getAmount();
		}
		return amount;
		*/
		return bettingAmount;
	}
	
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
	int getId() { return id; }
};

#endif // PLAYER_H