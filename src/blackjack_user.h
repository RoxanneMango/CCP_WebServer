#ifndef BLACKJACK_USER_H
#define BLACKJACK_USER_H

#include <vector>

#include "user.h"
#include "chip.h"
#include "card.h"

class BlackjackUser : public User
{
private:
public:
	BlackjackUser(User & user);
	BlackjackUser(int id, std::string name);
	
	virtual ~BlackjackUser();

	std::vector<Chip> chips;
	std::vector<Card> hand[2];

	double bettingAmount = 0;
	double insuranceAmount = 0;
	
	//
	void addChips(BlackjackUser & user, Chip chip);
	void removeChips(BlackjackUser & user, Chip chip);
	//
	void resetHand();
	
	double getBettingAmount();
	
	static BlackjackUser * create(User & user)
	{
		return new BlackjackUser(user);
	}
	static BlackjackUser * create(int id, std::string name)
	{
		return new BlackjackUser(id, name);
	}
};

#endif // BLACKJACK_USER_H