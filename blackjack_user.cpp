#include "blackjack_user.h"

BlackjackUser::BlackjackUser(User * user) : User(*user), userHandle(user)
{}
BlackjackUser::BlackjackUser(User user) : User(user), userHandle(NULL)
{}
BlackjackUser::~BlackjackUser()
{
	if(userHandle != NULL)
		delete this;
}

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

double
BlackjackUser::getBettingAmount()
{
	return bettingAmount;
}