#include "black_jack.h"

BlackJack::BlackJack(int id, unsigned int numOfDecks) : numOfDecks(numOfDecks)
{
	this->id = id;
	name = "Black Jack";
	minUsers = 1;
	currentUsers = 0;
	maxUsers = 1;
	running = false;
}
BlackJack::~BlackJack()
{}

Card
BlackJack::getRandomCard()
{
	srand(time(0));
	int index = rand() % numOfDecks;
	return cardDecks[index].getRandomCard();
}

std::string
BlackJack::getHands()
{	
	// SUIT
	// 0 H
	// 1 C
	// 2 D
	// 3 S
	
	// PICTURE
	// 0 NONE
	// 1 J
	// 2 Q
	// 3 K
	// 4 A
	
	// split on ; to get individual player hands
	// split on : to get individual player cards, where index 0 is always player name
	// split on , to get individual player card info (value, suit, true value)
	// <name>:<value/picture>,<suit>,<true_value>:<value/picture>,<suit>,<true_value>;<name>,<value/picture>,<suit>,<true_value>
	
	std::string hands;
	for(unsigned int i = 0; i < users.size(); ++i)
	{
		if(users[i].hand[0].size() == 0) return "!OK";

		hands.append(users[i].getName());
		hands.append(":");
		
		for(unsigned int j = 0; j < users[i].hand[0].size(); ++j)
		{
			int pv = users[i].hand[0][j].getPictureValue();
			int sv = users[i].hand[0][j].getSuitValue();

			if(pv==0) hands.append(std::to_string(users[i].hand[0][j].getValue()));
			else hands.append(pv==1?"J":pv==2?"Q":pv==3?"K":"A");
			hands.append(",");
			hands.append(sv==0?"H":sv==1?"C":sv==2?"D":"S");
			hands.append(",");
			hands.append(std::to_string(users[i].hand[0][j].getValue()));
			if(j+1 != users[i].hand[0].size()) hands.append(":");
		}
		if(users[i].hand[1].size() > 0)
		{
			for(unsigned int j = 0; j < users[i].hand[1].size(); ++j)
			{
				int pv = users[i].hand[0][j].getPictureValue();
				int sv = users[i].hand[0][j].getSuitValue();

				if(pv==0) hands.append(std::to_string(users[i].hand[0][j].getValue()));
				else hands.append(pv==1?"J":pv==2?"Q":pv==3?"K":"A");
				hands.append(",");
				hands.append(sv==0?"H":sv==1?"C":sv==2?"D":"S");
				hands.append(",");
				hands.append(std::to_string(users[i].hand[0][j].getValue()));
				if(j+1 != users[i].hand[0].size()) hands.append(":");
			}
		}
		if(i+1 != users[i].hand[0].size()) hands.append(";");
	}
	printf("hands: %s\n", &hands[0]);
	return hands;
}

void
BlackJack::init()
{
	printf("init() . . .\n");

	// dealer is always players[players.size()-1]
	users.push_back(User(-1, "Dealer", "", "", true));
	
	for(unsigned int i = 0; i <= numOfDecks; ++i)
	{
		cardDecks.push_back(DeckOfCards());
	}
	shuffleDecks();

	running = true;
	isUserTurn = true;
	insuranceBet = false;
	isWon = false;
	isDone = false;
	
	try
	{
		if(thread.joinable())
		{
			thread.detach();
		}
		thread = std::thread(&BlackJack::run, this);
	}
	catch (std::exception const & exception)
	{
		printf("Exception: %s\n", exception.what());
		return;
	}
	isReady = true;
	
	phase = 0;
}

void
BlackJack::bettingPhase()
{
	if(users[0].getBettingAmount() > 0)
	{
		printf("Bet placed: %.2f\n", users[0].getBettingAmount());
		isReady = false;
		phase+=1;
	}
}

void
BlackJack::dealingPhase()
{
	printf("Dealing phase . . .\n");
	
	// give players two random cards; dealer one face up, one down
	users[0].hand[0].push_back(getRandomCard());
	users[users.size()-1].hand[0].push_back(getRandomCard());
	users[0].hand[0].push_back(getRandomCard());
	users[users.size()-1].hand[0].push_back(getRandomCard());
	users[users.size()-1].hand[0][1].hide();

	for(unsigned int i = 0; i < users.size(); ++i)
	{
		for(unsigned int j = 0; j < users[i].hand[0].size(); ++j)
		{
			users[i].hand[0][j].print();
			printf("\n");
		}
	}
	printf("\n");

	isReady = true;
//	isDone = false;

	phase += 1;
}

void
BlackJack::hittingPhase()
{
	printf("Hitting phase . . .\n");
	
//	isDone = false;	
	phase+=1;
}

void
BlackJack::settlementPhase()
{
	stop();
}

void
BlackJack::run()
{
	try
	{
		while(running)
		{
			if(isDone)
			{
				if(phase==0)
				{
					bettingPhase();
				}
				else if(phase==1)
				{
					dealingPhase();
				}
				else if(phase==2)
				{
					hittingPhase();	
				}
				else if(phase==3)
				{
					settlementPhase();
				}
				// don't kill the cpu
			}
			sleep(1);
		}
	}
	catch (std::exception const & ex)
	{
		printf("Exception: %s\n", ex.what());
	}
}

std::string
BlackJack::input(Param param)
{
	try
	{
		if(strcmp(param.keys[0], "start") == 0)
		{
			if(running)
				throw "GAME_ALREADY_RUNNING";
			if(currentUsers < minUsers)
				throw "NOT_ENOUGH_USERS";
			if(users[0].getToken() != param.values[0])
				throw "INVALID_TOKEN";
			init();
			return "START";
		}				
		if(strcmp(param.keys[0], "stop") == 0)
		{
			if(!running)
				throw "GAME_ALREADY_STOPPED";
			if(users[0].getToken() != param.values[0])
				throw "INVALID_TOKEN";
			stop();
			return "STOP";
		}
		if(strcmp(param.keys[0], "bet") == 0)
		{
			if( (param.keys.size() < 2) || (param.values.size() < 2) )
				throw "NOT_ENOUGH_PARAMS";
			if(!running || phase)
				throw "COULD_NOT_PLACE_BET";
			if((strcmp(param.keys[1], "token")) || (users[0].getToken() != param.values[1]))
				throw "INVALID_TOKEN";
			if(atof(param.values[0]) <= 0)
				throw "BETTING_AMOUNT_INVALID";
			users[0].bettingAmount = atof(param.values[0]);
			isReady = false;
			isDone = true;
			return "OK";
		}
		if(strcmp(param.keys[0], "gethands") == 0)
		{
			while(!isReady)
			{
				sleep(0.25);
			}
			
			if(users[0].getToken() != param.values[0])
				throw "INVALID_TOKEN";
			if(!running || (phase < 1))
				throw "COULD_NOT_GET_HAND";
			std::string response = getHands();
			//isReady = true;
			isDone = true;
			return response;
		}
		if(strcmp(param.keys[0], "getphase") == 0)
		{
			if((!running) || (users[0].getToken() != param.values[0]))
				return "-1";
			return std::to_string(phase);
		}
	}
	catch(const char * exception)
	{
		return exception;
	}
	return "OK";
}

void
BlackJack::stop()
{
	printf("stop() . . .\n");
	running = false;
	cardDecks.clear();
	users.clear();
	bets.clear();
	currentUsers = 0;

}

bool
BlackJack::isRunning()
{
	return running;
}

void
BlackJack::shuffleDecks()
{
	srand(time(0));
		
	int deck_index_1 = rand() % cardDecks.size();
	int deck_index_2 = rand() % cardDecks.size();

	int index_1 = rand() % cardDecks[deck_index_1].getNumOfCards();
	int index_2 = rand() % cardDecks[deck_index_2].getNumOfCards();
	
	Card card = cardDecks[deck_index_1].deck[index_1];

	unsigned int shuffleIntensity = (cardDecks.size() * 52) * 3;

	for(unsigned int i = 0; i < shuffleIntensity; ++i)
	{
		cardDecks[deck_index_1].deck[index_1] = cardDecks[deck_index_2].deck[index_2];
		cardDecks[deck_index_2].deck[index_2] = card;
		//
		deck_index_1 = rand() % cardDecks.size();
		deck_index_2 = rand() % cardDecks.size();
		//
		index_1 = rand() % cardDecks[deck_index_1].getNumOfCards();
		index_2 = rand() % cardDecks[deck_index_2].getNumOfCards();
		//
		card = cardDecks[deck_index_1].deck[index_1];
	}
}