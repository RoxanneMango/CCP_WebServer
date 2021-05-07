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
	int maxCardValue = 11;
	
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
	char getPicture();
	
	void setSuit(Suit suit);
	int getSuitValue();
	char getSuit();
	
	void setMinCardValue(int value);
	void setMaxCardValue(int value);
	//
	int getMinCardValue();
	int getMaxCardValue();
	
	void hide();
	void expose();
	bool isHidden();
	
	void print();
	
	bool operator == (Card & card)  { return (this->value == card.getValue()); }
	bool operator == (int value)    { return (this->value == value);           }
	bool operator == (char picture) { return (getPicture() == picture);        }
};

#endif // CARD_H