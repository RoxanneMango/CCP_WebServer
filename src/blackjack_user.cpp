#include "blackjack_user.h"

BlackjackUser::BlackjackUser(User & user) : User(user)
{}

BlackjackUser::BlackjackUser(int id, std::string name) : User(id, name)
{}

BlackjackUser::~BlackjackUser()
{}

void
BlackjackUser::addChips(BlackjackUser & user, Chip chip)
{
	user.chips.push_back(chip);
}

void
BlackjackUser::removeChips(BlackjackUser & user, Chip chip)
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

void
BlackjackUser::resetHand()
{
	hand[0].clear();
	hand[1].clear();
}

void
BlackjackUser::resetFlags()
{
	isNatural = false;
	isDoubleDown = false;
	isSplit = false;
	isSurrender = false;
	isWon[0] = false;
	isWon[1] = false;
	isLose[0] = false;
	isLose[1] = false;
	isHit = false;
	isStand = false;
	canSplit = false;
	canInsurance = false;
}

double
BlackjackUser::getBettingAmount(unsigned int hand)
{
	return bettingAmount[hand];
}

int
BlackjackUser::getSettlement()
{
	// -1	not settled
	//
	//	0	loss
	//	1	win
	//	2	draw
	//	3	loss loss
	//	4	draw loss
	//	5	loss draw
	//	6	draw draw
	//	7	win loss
	//	8	loss win
	//	9	win win
	
	if(!hand[1].size())
	{
		if( isLose[0] && !isWon[0] )
			return 0;
		if( !isLose[0] && isWon[0] )
			return 1;
		return 2;
	}
	else
	{
		if( isLose[0] && isLose[1] && !isWon[0] && !isWon[1] )
			return 3;
		if( isLose[0] && isWon[0] && isLose[1] && !isWon[1] )
			return 4;
		if( isLose[0] && !isWon[0] && isLose[1] && isWon[1] )
			return 5;
		if( isLose[0] && isLose[1] && isWon[0] && isWon[1] )
			return 6;
		if( !isLose[0] && isWon[0] && !isWon[1] && isLose[1] )
			return 7;
		if( isLose[0] && !isWon[0] && isWon[1] && !isLose[1] )
			return 8;
	}
	return 9;
}