#include "dealer.h"

Dealer::Dealer(std::vector<DeckOfCards> * cards, unsigned int * numOfDecks) : 
	BlackjackUser(-1, "Dealer"), cards(cards), numOfDecks(numOfDecks)
{}
Dealer::~Dealer()
{}

void
Dealer::shuffle()
{
	srand(time(0));
		
	int deck_index_1 = rand() % cards->size();
	int deck_index_2 = rand() % cards->size();

	int index_1 = rand() % (*cards)[deck_index_1].getNumOfCards();
	int index_2 = rand() % (*cards)[deck_index_2].getNumOfCards();
	
	Card card = (*cards)[deck_index_1].deck[index_1];

	unsigned int shuffleIntensity = (cards->size() * 52) * 3;

	for(unsigned int i = 0; i < shuffleIntensity; ++i)
	{
		(*cards)[deck_index_1].deck[index_1] = (*cards)[deck_index_2].deck[index_2];
		(*cards)[deck_index_2].deck[index_2] = card;
		//
		deck_index_1 = rand() % cards->size();
		deck_index_2 = rand() % cards->size();
		//
		index_1 = rand() % (*cards)[deck_index_1].getNumOfCards();
		index_2 = rand() % (*cards)[deck_index_2].getNumOfCards();
		//
		card = (*cards)[deck_index_1].deck[index_1];
	}
}

void
Dealer::deal(std::vector<void *> * users)
{
	for(void * user : *users)
	{
		dealCard((BlackjackUser *)user, 0);
		dealCard((BlackjackUser *)user, 0);
	}
	hand[0][1].hide();
}

void
Dealer::split(BlackjackUser * user)
{
	// make two hands with one card each
	Card card = user->hand[0][0];
	user->hand[0].pop_back();
	user->hand[1].push_back(card);
	
	// give each hand a card
	dealCard(user, 0);
	dealCard(user, 1);
}

void
Dealer::dealCard(BlackjackUser * user, int handIndex)
{
	srand(time(0));
	int index = rand() % ((*numOfDecks)-1);
	user->hand[handIndex].push_back((*cards)[index].getCard());	
}

/*
void
Dealer::dealCard()
{
	srand(time(0));
	int index = rand() % ((*numOfDecks)-1);
	hand.push_back((*cards)[index].getCard());
}
*/

void
Dealer::revealCard()
{
	hand[0][1].expose();	
}