#ifndef BLACKJACK_USER_H
#define BLACKJACK_USER_H

#include <vector>

#include "user.h"
#include "chip.h"
#include "card_hand.h"

class BlackjackUser : public User
{
private:
public:
	BlackjackUser(User & user);
	BlackjackUser(int id, std::string name);
	
	virtual ~BlackjackUser();

	// The ---- --- of flags
	bool isSplit = false;
	bool isDoubleDown = false;
	bool isNatural = false;
	bool isSurrender = false;
	bool isHit = false;
	bool isStand = false;
	//
	bool canSplit = false;
	bool canInsurance = false;
	//
	bool isWon[2] = {0};
	bool isLose[2] = {0};

	double bettingAmount[2] = {0};
	double insuranceAmount = 0;

	void resetFlags();

	std::vector<Chip> chips;
	CardHand hand[2];
	
	//
	void addChips(BlackjackUser & user, Chip chip);
	void removeChips(BlackjackUser & user, Chip chip);
	//
	void resetHand();
	
	double getBettingAmount(unsigned int hand);
	
	int getSettlement();
	
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