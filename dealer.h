#ifndef DEALER_H
#define DEALLER_H

#include "blackjack_user.h"
#include "deck_of_cards.h"

class Dealer
{
private:
	std::vector<DeckOfCards> * cards;
	unsigned int * numOfDecks;
public:
	Dealer(std::vector<DeckOfCards> * cards, unsigned int * numOfDecks);
	~Dealer();

	std::vector<Card> hand;
	
	void shuffle();
	void deal(std::vector<BlackjackUser *> * users);
	void split(BlackjackUser * user);
	void dealCard();
	void dealCard(BlackjackUser * user, int handIndex);
	void revealCard();
};

#endif // DEALER_H