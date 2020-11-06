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

void
BlackJack::init()
{
	printf("init() . . .\n");
	
	for(unsigned int i = 0; i <= numOfDecks; ++i)
	{
		cardDecks.push_back(DeckOfCards());
	}
	shuffleDecks();

//	user.setBalance(40.00);
//	dealer.setBalance(27.50);
	
//	printf("User balance : %.2f %s\n", user.getBalance(), &(user.getCurrency()[0]));
//	printf("SuperUser balance : %.2f %s\n", dealer.getBalance(), &(dealer.getCurrency()[0]));

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
	catch (std::exception const & ex)
	{
		printf("Exception: %s\n", ex.what());
		return;
	}
}

void
BlackJack::bettingPhase()
{
/*
	while(!isDone)
	{
		sleep(1);
	}
	isDone = false;
*/	
//	if(user.getBettingAmount() < 1)
//	{
//		printf("Can't bet less than 1!\n");
//		bettingPhase();
//	}
}

bool
BlackJack::dealingPhase()
{
/*
	while(!isDone)
	{
		sleep(1);
	}
	isDone = false;
*/	
	return 0;
}

bool
BlackJack::hittingPhase()
{
/*
	while(!isDone)
	{
		sleep(1);
	}
	isDone = false;
*/
	return 0;
}

bool
BlackJack::settlementPhase()
{
	return 0;
}

void
BlackJack::run()
{
	try
	{
		while(running)
		{
			bettingPhase();
			dealingPhase();
			hittingPhase();
			settlementPhase();
			
			// don't kill the cpu
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
	for(unsigned int i = 0; i < param.keys.size(); ++i)
	{
		if(strcmp(param.keys[i], "blackjack") == 0)
		{
			if(strcmp(param.values[i], "start") == 0)
			{
				if(!running)
				{
					init();
					return "START";
				}
				else
				{
					return "GAME_ALREADY_RUNNING";
				}
			}				
			if(strcmp(param.values[i], "stop") == 0)
			{
				if(running)
				{
					stop();
					return "STOP";
				}
				else
				{
					return "GAME_ALREADY_STOPPED";
				}
			}
			if(strcmp(param.values[i], "command") == 0)
			{
				if(running)
				{
					stop();
					return "STOP";
				}
				else
				{
					return "GAME_ALREADY_STOPPED";
				}
			}
			if(strcmp(param.keys[i], "bet") == 0)
			{
				if(running)
				{
					// stoi is C++11
					// ID user
					// player.bettingAmount = atoi(param.values[i]);
					return "BET_PLACED";
				}
				else
				{
					return "COULD_NOT_PLACE_BET";
				}
			}
		}
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