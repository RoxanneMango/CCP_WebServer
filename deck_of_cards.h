#ifndef DECK_OF_CARDS_H
#define DECK_OF_CARDS_H

#include <vector>
#include <stdlib.h>
#include <time.h>

#include "card.h"

class DeckOfCards
{
private:
	int numOfCards;

public:
	DeckOfCards(unsigned int numOfCards = 52);
	~DeckOfCards();
	
	std::vector<Card> deck;
	
	void shuffle();
	void addCard(Card card);
	
	Card getCard(unsigned int index);
	Card getRandomCard();
	int getNumOfCards();
	
	void printCards();
};

#endif // DECK_OF_CARDS_H