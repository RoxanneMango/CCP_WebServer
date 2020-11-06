#include "deck_of_cards.h"

DeckOfCards::DeckOfCards(unsigned int numOfCards) : numOfCards(numOfCards)
{	
	for(int i = 2; i <= 10; ++i)
	{
		deck.push_back( Card(i, Card::Suit::HEARTS) );
	}
	deck.push_back( Card(Card::Picture::JACK, Card::Suit::HEARTS) );
	deck.push_back( Card(Card::Picture::QUEEN, Card::Suit::HEARTS) );
	deck.push_back( Card(Card::Picture::KING, Card::Suit::HEARTS) );
	deck.push_back( Card(Card::Picture::ACE, Card::Suit::HEARTS) );
	
	for(int i = 2; i <= 10; ++i)
	{
		deck.push_back( Card(i, Card::Suit::CLUBS) );
	}
	deck.push_back( Card(Card::Picture::JACK, Card::Suit::CLUBS) );
	deck.push_back( Card(Card::Picture::QUEEN, Card::Suit::CLUBS) );
	deck.push_back( Card(Card::Picture::KING, Card::Suit::CLUBS) );
	deck.push_back( Card(Card::Picture::ACE, Card::Suit::CLUBS) );
	
	for(int i = 2; i <= 10; ++i)
	{
		deck.push_back( Card(i, Card::Suit::DIAMONDS) );
	}
	deck.push_back( Card(Card::Picture::JACK, Card::Suit::DIAMONDS) );
	deck.push_back( Card(Card::Picture::QUEEN, Card::Suit::DIAMONDS) );
	deck.push_back( Card(Card::Picture::KING, Card::Suit::DIAMONDS) );
	deck.push_back( Card(Card::Picture::ACE, Card::Suit::DIAMONDS) );

	for(int i = 2; i <= 10; ++i)
	{
		deck.push_back( Card(i, Card::Suit::SPADES) );
	}	
	deck.push_back( Card(Card::Picture::JACK, Card::Suit::SPADES) );
	deck.push_back( Card(Card::Picture::QUEEN, Card::Suit::SPADES) );
	deck.push_back( Card(Card::Picture::KING, Card::Suit::SPADES) );
	deck.push_back( Card(Card::Picture::ACE, Card::Suit::SPADES) );	
}

DeckOfCards::~DeckOfCards()
{}

void
DeckOfCards::shuffle()
{
	srand(time(0));
	
	int index_1 = rand() % numOfCards;
	int index_2 = rand() % numOfCards;
	
	Card card = deck[index_1];

	for(int i = 0; i < numOfCards * 3; ++i)
	{
		deck[index_1] = deck[index_2];
		deck[index_2] = card;

		index_1 = rand() % numOfCards;
		index_2 = rand() % numOfCards;
		
		card = deck[index_1];
	}
}

void
DeckOfCards::addCard(Card card)
{
	deck.push_back(card);
}

Card
DeckOfCards::getCard(unsigned int index)
{
	Card card = deck[index];
	deck.erase(deck.begin() + index);
	return card;
}

Card
DeckOfCards::getRandomCard()
{
	srand(time(0));
	
	int index = rand() % numOfCards;
	Card card = deck[index];
	deck.erase(deck.begin() + index);
	
	return card;
}

int
DeckOfCards::getNumOfCards()
{
	return numOfCards;
}

void
DeckOfCards::printCards()
{
	for(int i = 0; i < numOfCards; ++i)
	{
		deck[i].print();
	}
}