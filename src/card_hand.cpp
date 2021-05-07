#include "card_hand.h"

CardHand::CardHand()
{}
CardHand::~CardHand()
{}

std::vector<Card>::iterator
CardHand::begin()
{
	return cards.begin();
}
std::vector<Card>::iterator
CardHand::end()
{
	return cards.end();
}

void
CardHand::push_back(Card card)
{
	cards.push_back(card);
}
void
CardHand::pop_back()
{
	cards.pop_back();
}

void
CardHand::clear()
{
	cards.clear();
}

unsigned int
CardHand::size()
{
	return cards.size();
}

int
CardHand::sum()
{
	int sum = 0;
	for(Card card : cards)
	{
		sum += card.getValue();
	}
	return sum;
}

std::string
CardHand::toString()
{
	std::string str = "";

	for(unsigned int j = 0; j < cards.size(); ++j)
	{
		int pv = cards[j].getPictureValue();
		int sv = cards[j].getSuitValue();

		// Second card dealt is face down -- hidden
		if(cards[j].isHidden())
		{
			str.append(std::string("hidden"));
			continue;
		}
		
		if(pv==0) str.append(std::to_string(cards[j].getValue()));
		else str.append(pv==1?"J":pv==2?"Q":pv==3?"K":"A");
		str.append(",");
		str.append(sv==0?"H":sv==1?"C":sv==2?"D":"S");
		str.append(",");
		str.append(std::to_string(cards[j].getValue()));
		if(j+1 != cards.size()) str.append(":");
	}
	str.append(";");

	return str;
}
