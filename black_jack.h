#ifndef BLACK_JACK_H
#define BLACK_JACK_H

#include "sleep.h"
#include "thread.h"
#include "game.h"
#include "chip.h"
#include "deck_of_cards.h"
#include "blackjack_user.h"
#include "dealer.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
*
* -=- Blackjac++ gameplay -=-
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* // betting phase
* 0. players place bet
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* // dealing phase
* 1. player get first card face up
* 2. dealer get first card face up
* 3. player get second card face up
* 4. dealer get second card face down
*	4.1 if card is an ace, players may make a side bet of up to half their original 
*		bet that the dealer's second card is a '10' (insurance)
*		4.1.1 if all such bets are placed, the dealer will look at the face down card
*		4.1.2 if it is a '10', the card is placed face up and players who betted that 
*			  it would be get double the amount of their half-bet
*	4.2 if dealer face up card is either '10' or 'ace', they may check value of face 
*		down card to see if they have blackjack
*		4.2.1 before the dealer checks for blackjack, player gets chance to surrender
*			4.2.1.1 players who surrender get back half their bet and leave the game
* 	4.3 if dealer has blackjack and no one else -> everyone loses
* 	4.4 if dealer has blackjack and one or more has blackjack as well, everyone but 
*		they lose; they get back their bet (standoff)
*	4.5 if player has blackjack and dealer does not -> they win and also receive a 
*		1.5 turnout bonus (natural)
* 	4.6 game ends i
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* // hitting phase
* 5. if player has two cards with the same value, they may choose to split them 
*	 (splitting pairs)
*	5.1 each seperate card becomes its own hand, and player must add same betting amount 
*		to second hand
*	5.2 if player is splitting aces, only one additional card is given and player cannot 
*		ask for more cards
*		5.2.1 if the given card is a '10' and thus results in a blackjack, only a normal 
*			  pay-off is given, no bonus pay-off is given
* 6. Dealer will ask each player if they want another card (hit), or do nothing (stand), 
*	 going clockwise
* 	6.1 if 2 player cards have a total value of 9, 10 or 11 player can double their bet 
*		(doubling down)
*		6.1.1 player will receive one more card, faced down and only turned up at the very 
*			  end
*		6.1.2 player cannot ask for more cards (stand)
*	6.2 if player split pairs previously, they will start with their left hand first, 
*		finish that, then start with their right hand and finish that
*	6.3 if player asks for another card and exceeds a total value of 21, they lose (bust) 
*		and the dealer collects their bet
*	6.4 if player no longer want cards it is the next player's turn (stand)
*	6.5 once all players are done it is the dealer's turn
*		6.5.1 the faced down card is turned over
*		6.5.2 the dealer will keep asking for cards (hitting) until they hold a total 
*			  value of 17 or higher
*		6.5.3 if the dealer exceeds 21 (bust) they pay each player who didn't bust their 
*			  betting amount and the game ends
*
*	// settlement phase
*	6.6 the dealer will pay everyone who has a higher total value than them but didn't 
*		bust their betting amount
*		6.6.1 if a player has the same amount as the dealer, no chips are paid out or 
*			  collected (stand-off)
*
*	// reshuffling
*	7. used cards are stashed away after each play
*	8. dealer will continue to deal from the deck until a given point, after which all 
*	   cards will be merged back together and reshuffled
* 
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class BlackJack : public Game
{
private:	
	std::thread thread;
	
	bool running = false;
	//
	bool isReady = false;
	bool isDone = false;
	//
	bool isSplit = false;
	bool isDoubleDown = false;
	bool isNatural = false;
	bool isSurrender = false;
	bool isHit = false;
	bool isStand = false;
	//
	bool canSplit = false;
	bool canInsurance = false;
	//
	bool isWon[2] = {0};
	bool isLose[2] = {0};
	
	int userTurnIndex = 0;
	
	unsigned int numOfDecks;
	std::vector<DeckOfCards> cardDecks;
	
	Dealer dealer;
	
	std::vector<double> bets;

	double maxBettingAmount = 4000;
	double bettingAmount[2] = {0};
	double insuranceAmount = 0;
	
	void bettingPhase();
	void dealingPhase();
	void insurancePhase();
	void splittingPhase();
	void hittingPhase();
	void settlementPhase();
	
	unsigned int currentUserId;
	
	enum State { BETTING = 0, DEALING, INSURANCE, SPLITTING, HITTING, SETTLEMENT };
	
	State state = State::BETTING;
	
	Card getCard();
	std::string getHands();
	int getSettlement();
	
public:
	BlackJack(int id, unsigned int numOfDecks);
	~BlackJack();
	
	std::string input(Param param);
	
	void init();
	void run();
	void stop();
	
	bool isRunning();
	
	void addUser(void * user) override;
};

#endif // BLACK_JACK_H