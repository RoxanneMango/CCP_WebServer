#ifndef CARD_H
#define CARD_H

#include <string>
#include <stdio.h>

class Card
{
	int suit = 0;
	int picture = 0;
	int value = 0;

	int minCardValue = 1;
	int maxCardValue = 10;
	
	bool hidden = false;
public:

	enum Suit { HEARTS = 0, CLUBS, DIAMONDS, SPADES };
	enum Picture { NONE = 0, JACK, QUEEN, KING, ACE };

	Card(int value, Suit suit);
	Card(Picture picture, Suit suit);
	~Card();
	
	void setValue(int value);
	int getValue();
	
	void setPictureValue(Picture picture);
	int getPictureValue();
	
	void setSuit(Suit suit);
	int getSuit();
	
	void setMinCardValue(int value);
	void setMaxCardValue(int value);
	//
	int getMinCardValue();
	int getMaxCardValue();
	
	void hide();
	void expose();
	bool isHidden();
	
	void print();
	
};

#endif // CARD_H