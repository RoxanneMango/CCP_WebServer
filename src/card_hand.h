#ifndef CARD_HAND_H
#define CARD_HAND_H

#include <vector>
#include <string>

#include "card.h"

class CardHand
{
private:
	std::vector<Card> cards;
public:
	CardHand();
	~CardHand();
	
	std::vector<Card>::iterator begin();
	std::vector<Card>::iterator end();
	
	void push_back(Card card);
	void pop_back();
	
	void clear();
	unsigned int size();
	int sum();
	
	std::string toString();
	
	Card & operator[](unsigned int i) { return cards[i]; }
	
	bool operator==(CardHand & cards) { return sum() == cards.sum(); }
	bool operator!=(CardHand & cards) { return sum() != cards.sum(); }
	bool operator< (CardHand & cards) { return sum() <  cards.sum(); }
	bool operator<=(CardHand & cards) { return sum() <= cards.sum(); }
	bool operator> (CardHand & cards) { return sum() >  cards.sum(); }
	bool operator>=(CardHand & cards) { return sum() >= cards.sum(); }

	bool operator==(int num) { return sum() == num; }
	bool operator!=(int num) { return sum() != num; }
	bool operator< (int num) { return sum() <  num; }
	bool operator<=(int num) { return sum() <= num; }
	bool operator> (int num) { return sum() >  num; }
	bool operator>=(int num) { return sum() >= num; }
};

#endif // CARD_HAND_H