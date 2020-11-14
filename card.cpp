#include "card.h"

Card::Card(int value, Suit suit)
{
	try
	{
		if(value < minCardValue)
		{
			throw "Card value cannot be smaller than 1.";
		}
		else if(value > maxCardValue)
		{
			throw "Card value cannot be greater than 10.";
		}
		else
		{
			this->value = value;
		}
		
		if(suit < 0)
		{
			throw "Suit value cannot be smaller than 0.";
		}
		else if(suit > 4)
		{
			throw "Suit value cannot be greater than 4.";
		}
		else
		{
			this->suit = suit;
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s Destroying object again . . .\n", exception);
		this->Card::~Card();
	}
}

Card::Card(Picture picture, Suit suit)
{
	try
	{
		if(picture < 0)
		{
			throw "Picture value cannot be smaller than 1.";
		}
		else if(picture > 4)
		{
			throw "Picture value cannot be greater than 4.";
		}
		else
		{
			this->picture = picture;
			this->value = 10;
		}
		
		if(suit < 0)
		{
			throw "Suit value cannot be smaller than 0.";
		}
		else if(suit > 4)
		{
			throw "Suit value cannot be greater than 4.";
		}
		else
		{
			this->suit = suit;
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s Destroying object again . . .\n", exception);
		this->Card::~Card();
	}
}

Card::~Card()
{}

void
Card::setValue(int value)
{
	try
	{
		if(value < minCardValue)
		{
			throw "Card value cannot be smaller than 1.";
		}
		else if(value > maxCardValue)
		{
			throw "Card value cannot be greater than 10.";
		}
		else
		{
			this->value = value;
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}

int
Card::getValue()
{
	return value;
}

void
Card::setPictureValue(Picture picture)
{
	try
	{
		if(picture < 0)
		{
			throw "Picture value cannot be smaller than 0.";
		}
		else if(picture > 4)
		{
			throw "Picture value cannot be greater than 4.";
		}
		else
		{
			this->picture = picture;
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}

int
Card::getPictureValue()
{
	return picture;
}

char
Card::getPicture()
{
	if(picture == 1) return 'J';
	if(picture == 2) return 'Q';
	if(picture == 3) return 'K';
	if(picture == 4) return 'A';
	return 'N';
}

void
Card::setSuit(Suit suit)
{
	try
	{
		if(suit < 0)
		{
			throw "Suit value cannot be smaller than 0.";
		}
		else if(suit > 4)
		{
			throw "Suit value cannot be greater than 4.";
		}
		else
		{
			this->suit = suit;
		}
	}
	
	catch(const char * exception)
	{
		printf("Exception : %s\n", exception);
	}
}

int
Card::getSuitValue()
{
	return suit;
}

char
Card::getSuit()
{
	if(suit == 0) return 'H';
	if(suit == 1) return 'C';
	if(suit == 2) return 'D';
	return 'S';
}

void
Card::setMinCardValue(int value)
{
	minCardValue = value;
}
void
Card::setMaxCardValue(int value)
{
	maxCardValue = value;
}
int
Card::getMinCardValue()
{
	return minCardValue;
}
int
Card::getMaxCardValue()
{
	return maxCardValue;
}

void
Card::hide()
{
	hidden = true;
}
void
Card::expose()
{
	hidden = false;
}
bool
Card::isHidden()
{
	return hidden;
}


void
Card::print()
{	
	std::string s = suit == 0 ? "Hearts" : suit == 1 ? "Clubs" : suit == 2 ? "Diamonds" : "Spades";
	std::string p = picture == 0 ? "" : picture == 1 ? "Jack" : picture == 2 ? "Queen" : picture == 3 ? "King" : "Ace";

	if(picture)
	{
		printf("%s of %s ", &p[0], &s[0]);
	}
	else
	{
		printf("%d of %s ", value, &s[0]);
	}
}