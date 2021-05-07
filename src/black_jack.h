#ifndef BLACK_JACK_H
#define BLACK_JACK_H

#include "sleep.h"
#include "thread.h"
#include "game.h"
#include "chip.h"
#include "deck_of_cards.h"
#include "blackjack_user.h"
#include "dealer.h"

#define USER ((User *)(users[0]))
#define PLAYER ((BlackjackUser *)(users[0]))
#define PLAYERS(i) ((BlackjackUser *)(users[i]))

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


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* TODO
*
* 1.	Iterate over EXISTING database
* 2.	Fill in more database fields, such as BJ games lost and won, tied, etc
* 3.	Create API to ask user information from database
* 4.	Create API to let a user modify certain information such as display name and password
* 5.	Be able to email a user
* 6.	Add Encryption interface class
* 7.	Add Ceasar Cypher / hash Encryption class implementations
* 8.	Keep using same instance of decks of cards until they run out, only then reshuffle them
* 9.	Remove unnecesary comments
* 10.	Add meaningful comments
* 11.	Add save function
* 12.	Solve the stop() problem for the blackjack game; when to do it? timer?
* 13.	Change buffers to vector<char> or something similar for variable length
* 14.	Create isPlayingGame flag for User to prevent them from joining more than one game
* 15.	Move flags such as canSplit to the BlackjackUser class
* 16.	Check out rule of 5
* 17.	Figure out how to send images via the WebServer. uint8_t byte array?
* 18.	Test all the SETTLEMENT outcomes; whether they are handled correctly
* 19.	
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class BlackJack : public Game
{
private:	
	std::thread thread;
	
	bool running = false;
	bool isReady = false;
	bool isDone = false;

	int userTurnIndex = 0;
	double maxBettingAmount = 4000;

	unsigned int numOfDecks;
	std::vector<DeckOfCards> cardDecks;

	Dealer * dealer;
	
	void bettingPhase();
	void dealingPhase();
	void insurancePhase();
	void splittingPhase();
	void hittingPhase();
	void settlementPhase();
	
	enum State { BETTING = 0, DEALING, INSURANCE, SPLITTING, HITTING, SETTLEMENT };
	State state = State::BETTING;
	
	Card getCard();
	std::string getHands();

public:
	BlackJack(int id, unsigned int numOfDecks);
	~BlackJack();
	
	std::string input(Param param);
	
	void init();
	void run();
	void stop();
	
	bool isRunning();
	
	User * addUser(User * user) override;
};

#endif // BLACK_JACK_H