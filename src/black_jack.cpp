#include "black_jack.h"

BlackJack::BlackJack(int id, unsigned int numOfDecks) : 
	Game(id), numOfDecks(numOfDecks)
{
	name = "Black Jack";
	running = false;

	currentUsers = 0;
	minUsers = 1;
	maxUsers = 1;

	maxBettingAmount = 4000;
}
BlackJack::~BlackJack()
{}

Card
BlackJack::getCard()
{
	srand(time(0));
	int index = rand() % (numOfDecks-1);
	return cardDecks[index].getCard();
}

std::string
BlackJack::getHands()
{	
	// split on ; to get individual player hands
	// split on : to get individual player cards, where index 0 is always player name
	// split on , to get individual player card info (value, suit, true value)
	// <name>:<value/picture>,<suit>,<true_value>:<value/picture>,<suit>,<true_value>;<name>,<value/picture>,<suit>,<true_value>
	std::string hands;
	for(unsigned int i = 0; i < users.size(); ++i)
	{
		if(PLAYERS(i)->hand[0].size() == 0) return "!OK";
		hands.append(PLAYERS(i)->getName());
		hands.append(":");
		for(unsigned int k = 0; k < (1 + (PLAYERS(i)->hand[1].size() > 0)); ++k)
		{
			hands.append(PLAYERS(i)->hand[k].toString());
		}
	}
	return hands;
}

void
BlackJack::init()
{
	printf("init() . . .\n");
	
	for(unsigned int i = 0; i <= numOfDecks; ++i)
	{
		cardDecks.push_back(DeckOfCards());
	}
	dealer = Dealer::create(&cardDecks, &this->numOfDecks);
	users.push_back(dealer);

	dealer->shuffle();

	userTurnIndex = 0;
	running = true;
	isDone = false;
	//
	PLAYER->resetFlags();
	PLAYER->resetHand();

	dealer->resetFlags();
	dealer->resetHand();

	try
	{
		if(thread.joinable())
		{
			thread.detach();
		}
		thread = std::thread(&BlackJack::run, this);
	}
	catch(std::exception const & exception)
	{
		printf("Exception: %s\n", exception.what());
		return;
	}
	
	state = State::BETTING;
	isReady = true;
}

void
BlackJack::bettingPhase()
{
	if(!isReady)
	{
		if(PLAYER->bettingAmount[0] > 0.00)
		{
			USER->subtractBalance(PLAYER->bettingAmount[0]);
			isDone = true;
		}
		isReady = true;
	}
	if(isDone)
	{
		state = State::DEALING;
		USER->isReady = true;
		isReady = false;
	}
}

void
BlackJack::dealingPhase()
{
	if(!isReady)
	{
		dealer->deal(&users);
		
		PLAYER->canSplit = PLAYER->hand[0][0] == PLAYER->hand[0][1];
		PLAYER->canInsurance = (dealer->hand[0][0].getValue() == 11);
		PLAYER->isNatural = ((PLAYER->hand[0][0].getValue() + PLAYER->hand[0][1].getValue()) == 21);
		
		isDone = (dealer->hand[0][0].getValue() != 11);
		isReady = true;
	}
	if(PLAYER->isNatural)
	{
		isDone = false;
		isReady = false;
		state = State::SETTLEMENT;
	}
	else if(PLAYER->canInsurance)
	{
		isDone = false;
		state = State::INSURANCE;
	}
	else if(isDone)
	{	
		isDone = false;
		if(PLAYER->canSplit)
		{
			state = State::SPLITTING;
		}
		else
		{
			state = State::HITTING;			
		}
	}
}

void
BlackJack::insurancePhase()
{
	if(isDone)
	{
		isDone = false;
		dealer->revealCard();
		if(((dealer->hand[0][0].getValue() + dealer->hand[0][1].getValue()) == 21))
		{
			USER->isReady = false;
			state = State::SETTLEMENT;
		}
		else if(PLAYER->canSplit)
		{
			state = State::SPLITTING;
			isReady = true;
		}
		else
		{
			state = State::HITTING;	
		}
	}
	else if(!isReady)
	{
		if(PLAYER->insuranceAmount > 0.00)
		{
			isDone = true;
			isReady = false;
			USER->subtractBalance(PLAYER->insuranceAmount);
			USER->isReady = true;
		}
		else
		{
			isReady = true;
		}
	}
}

void
BlackJack::splittingPhase()
{	
	if(PLAYER->isSplit)
	{
		if(USER->getBalance() >= PLAYER->bettingAmount[0])
		{
			PLAYER->bettingAmount[1] = PLAYER->bettingAmount[0];
			USER->subtractBalance(PLAYER->bettingAmount[1]);
		}
		dealer->split(PLAYER);
		isReady = true;
		isDone = true;
	}
	if(isDone)
	{
		state = State::HITTING;
		isDone = false;
	}
}

void
BlackJack::hittingPhase()
{
	if(PLAYER->isDoubleDown)
	{
		USER->subtractBalance(PLAYER->bettingAmount[userTurnIndex]);
		PLAYER->bettingAmount[userTurnIndex] *= 2;
		dealer->dealCard(PLAYER, userTurnIndex);
		userTurnIndex += 1;
		isDone = (userTurnIndex > (PLAYER->hand[1].size() ? 1 : 0));
	}
	else if(PLAYER->isSurrender)
	{
		PLAYER->isSurrender = false;
		USER->addBalance(PLAYER->bettingAmount[userTurnIndex] / 2);
		PLAYER->isLose[userTurnIndex] = true;
		userTurnIndex += 1;
		isDone = (userTurnIndex > (PLAYER->hand[1].size() ? 1 : 0));
	}
	else if(PLAYER->isHit)
	{
		PLAYER->isHit = false;
		if((userTurnIndex == 0) || (userTurnIndex == 1 && PLAYER->hand[1].size()))
		{
			dealer->dealCard(PLAYER, userTurnIndex);
			if(PLAYER->hand[userTurnIndex] > 21)
			{
				bool canLowerAce = false;
				for(Card & card : PLAYER->hand[userTurnIndex])
				{
					if(card == 'A')
					{
						if(card == 11)
						{
							card.setValue(1);
							canLowerAce = true;
							break;
						}
					}
				}
				if(!canLowerAce)
				{
					PLAYER->isLose[userTurnIndex] = true;
					userTurnIndex += 1;
					isDone = (userTurnIndex > (PLAYER->hand[1].size() ? 1 : 0));
				}
			}
		}
	}
	else if(PLAYER->isStand)
	{
		PLAYER->isStand = false;
		if((userTurnIndex == 0) && PLAYER->hand[1].size())
		{
			userTurnIndex += 1;
		}
		else
		{
			isDone = true;
		}
	}
	
	if(isDone)
	{
		dealer->revealCard();
		isDone = false;
		isReady = false;
		if(!PLAYER->isLose[0] || !PLAYER->isLose[1])
		{
			while(dealer->hand[0] < 14)
			{
				dealer->dealCard((BlackjackUser *)dealer, 0);
			}
		}
		USER->isReady = false;
		state = State::SETTLEMENT;
	}
	else
	{
		isReady = true;
	}
}

void
BlackJack::settlementPhase()
{
	if(!isReady)
	{
		dealer->revealCard();

		if((dealer->hand[0] == 21) && (dealer->hand[0].size() == 2) && (PLAYER->insuranceAmount > 0.00))
		{
			USER->addBalance(PLAYER->insuranceAmount * 2);
			PLAYER->isLose[0] = true;
			PLAYER->isLose[1] = true;
		}
		
		if(!PLAYER->isLose[0])
		{
			if(dealer->hand[0] == PLAYER->hand[0]) // DRAW
			{
				PLAYER->isWon[0] = true;
				PLAYER->isLose[0] = true;
				USER->addBalance(PLAYER->bettingAmount[0] * (1 + PLAYER->isDoubleDown));
			}
			else if(PLAYER->isNatural)		// NATURAL
			{
				PLAYER->isWon[0] = true;
				USER->addBalance(PLAYER->bettingAmount[0] * 1.5);
			}
			else if((PLAYER->hand[0] > dealer->hand[0]) && (PLAYER->hand[0] <= 21)) // WIN
			{
				PLAYER->isWon[0] = true;
				USER->addBalance(PLAYER->bettingAmount[0] * (PLAYER->hand[0] == 21 ? 3 : 2));
			}
			else if(dealer->hand[0] > 21)
			{
				PLAYER->isWon[0] = true;
				USER->addBalance(PLAYER->bettingAmount[0] * (PLAYER->hand[0] == 21 ? 3 : 2));
			}
			else
			{
				PLAYER->isLose[0] = true;
			}
		}
		if(!PLAYER->isLose[1] && PLAYER->hand[1].sum())
		{
			if(dealer->hand[0] == PLAYER->hand[1]) // DRAW
			{
				PLAYER->isWon[1] = true;
				PLAYER->isLose[1] = true;
				USER->addBalance(PLAYER->bettingAmount[1] / (1 + PLAYER->isDoubleDown));
			}
			else if((PLAYER->hand[1] > dealer->hand[0]) && (PLAYER->hand[1] <= 21))	// WIN
			{
				PLAYER->isWon[1] = true;
				USER->addBalance(PLAYER->bettingAmount[1] * (PLAYER->hand[0] == 21 ? 3 : 2));
			}
			else if(dealer->hand[0] > 21)
			{
				PLAYER->isWon[1] = true;
				USER->addBalance(PLAYER->bettingAmount[0] * (PLAYER->hand[0] == 21 ? 3 : 2));
			}
			else
			{
				PLAYER->isLose[1] = true;
			}
		}
		isReady = true;
		USER->isReady = true;
	}
	if(isDone)
	{
		running = false;
	}
}

void
BlackJack::run()
{
	try
	{
		while(running)
		{
			switch(state)
			{
				case BETTING:
				{
					bettingPhase();
					break;
				}
				case DEALING:
				{
					dealingPhase();
					break;
				}
				case INSURANCE:
				{
					insurancePhase();
					break;
				}
				case SPLITTING:
				{
					splittingPhase();
					break;
				}
				case HITTING:
				{
					hittingPhase();
					break;
				}
				case SETTLEMENT:
				{
					settlementPhase();
					break;
				}
			}
			SLEEP(0.2);
		}
	}
	catch (std::exception const & ex)
	{
		printf("Exception: %s\n", ex.what());
	}
	stop();
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
			init();
			return "START";
		}				
		if(strcmp(param.keys[0], "stop") == 0)
		{
			if(!running)
				throw "GAME_ALREADY_STOPPED";
			running = false;
			return "STOP";
		}
		if(strcmp(param.keys[0], "bet") == 0)
		{
			if( (param.keys.size() < 2) || (param.values.size() < 2) )
				throw "NOT_ENOUGH_PARAMS";
			if(!running || state != State::BETTING)
				throw "COULD_NOT_PLACE_BET";
			if(atof(param.values[1]) <= 0)
				throw "BETTING_AMOUNT_INVALID";
			if(atof(param.values[1]) > maxBettingAmount)
				throw "BETTING_AMOUNT_TOO_MUCH";
			if(atof(param.values[1]) > PLAYER->getBalance())
				throw "NOT_ENOUGH_BALANCE";
			
			time_t timeOut = time(NULL) + 5; // 5 second timeout time
			while(!USER->isReady)
			{
				if(time(NULL) >= timeOut)
				{
					break;
				}
				SLEEP(0.1);
			}
			if(!USER->isReady)
				throw "USER_NOT_READY";

			PLAYER->isReady = false;
			PLAYER->bettingAmount[0] = atof(param.values[1]);
			isReady = false;
			return "OK";
		}
		if(strcmp(param.keys[0], "insure") == 0)
		{
			if( (param.keys.size() < 2) || (param.values.size() < 2) )
				throw "NOT_ENOUGH_PARAMS";
			if(!running || state != State::INSURANCE)
				throw "COULD_NOT_PLACE_INSURANCE";
			if(atof(param.values[1]) <= 0)
				throw "INSURANCE_AMOUNT_INVALID";
			if(atof(param.values[1]) > (PLAYER->bettingAmount[0] / 2))
				throw "INSURANCE_AMOUNT_TOO_BIG";

			time_t timeOut = time(NULL) + 5; // 5 second timeout time
			while(!USER->isReady)
			{
				if(time(NULL) >= timeOut)
				{
					break;
				}
				SLEEP(0.1);
			}
			if(!USER->isReady)
				throw "USER_NOT_READY";

			PLAYER->isReady = false;
			PLAYER->insuranceAmount = atof(param.values[1]);
			isReady = false;
			return "OK";
		}
		if(strcmp(param.keys[0], "split") == 0)
		{
			if( (param.keys.size() < 2) || (param.values.size() < 2) )
				throw "NOT_ENOUGH_PARAMS";
			if(!running || state != State::SPLITTING)
				throw "COULD_NOT_SPLIT_HAND";
			if(PLAYER->getBalance() < PLAYER->bettingAmount[0])
				throw "NOT_ENOUGH_BALANCE_FOR_SPLIT";
			PLAYER->isSplit = true;
			isReady = false;
			return "OK";
		}
		if(strcmp(param.keys[0], "hit") == 0)
		{
			if(!running || state != State::HITTING)
				throw "COULD_NOT_HIT_CARD";
			if(PLAYER->isLose[0] && PLAYER->isLose[1])
				throw "CANNOT_HIT_WHEN_LOSE";
			if(PLAYER->isHit)
				throw "ALREADY_HIT";
			PLAYER->isHit = true;
			isReady = false;
			return "HIT";
		}
		if(strcmp(param.keys[0], "stand") == 0)
		{
			if(!running || state != State::HITTING)
				throw "COULD_NOT_STAND";
			if(PLAYER->isStand)
				throw "ALREADY_STANDING";
			PLAYER->isStand = true;
			isReady = false;
			return "STAND";
		}
		if(strcmp(param.keys[0], "continue") == 0)
		{
			if(!running)
				throw "GAME_NOT_RUNNING";
			if(state != State::INSURANCE && state != State::SPLITTING)
				throw "COULD_NOT_CONTINUE";
			isDone = true;
			isReady = false;
			return "CONTINUE";
		}
		if(strcmp(param.keys[0], "surrender") == 0)
		{
			if(!running)
				throw "GAME_NOT_RUNNING";
			if(state != State::HITTING)
				throw "COULD_NOT_SURRENDER";
			PLAYER->isSurrender = true;
			isReady = false;
			return "SURRENDER";
		}
		if(strcmp(param.keys[0], "doubledown") == 0)
		{
			if(!running)
				throw "GAME_NOT_RUNNING";
			if(state != State::HITTING)
				throw "COULD_NOT_DOUBLE_DOWN";
			if(PLAYER->isDoubleDown)
				throw "ALREAD_DOUBLE_DOWN";
			if(PLAYER->getBalance() < PLAYER->bettingAmount[0])
				throw "NOT_ENOUGH_BALANCE_FOR_DOUBLE_DOWN";
			PLAYER->isDoubleDown = true;
			isReady = false;
			return "DOUBLE_DOWN";
		}
		if(strcmp(param.keys[0], "gethands") == 0)
		{
			time_t timeOut = time(0) + 5; // 10 second timeout time
			while( (!isReady || (state == State::BETTING)) )
			{
				if(time(0) >= timeOut)
				{
					break;
				}
				SLEEP(0.1);
			}
			if(!running || !isReady || (state == State::BETTING))
			{
				printf("Phase: %d\n", (int)state);
				throw "COULD_NOT_GET_HAND";
			}
			return getHands();
		}
		if(strcmp(param.keys[0], "getsettlement") == 0)
		{

			time_t timeOut = time(0) + 5; // 5 second timeout time
			while( (!isReady) )
			{
				if(time(0) >= timeOut)
				{
					break;
				}
				SLEEP(0.1);
			}
			if(!running || !isReady || (state != State::SETTLEMENT))
			{
				throw "-1";
			}
			return std::to_string(PLAYER->getSettlement());
		}
		if(strcmp(param.keys[0], "getbettingamount") == 0)
		{
			time_t timeOut = time(0) + 5; // 5 second timeout time
			while( (!isReady) )
			{
				if(time(0) >= timeOut)
				{
					break;
				}
				SLEEP(0.1);
			}
			if(!running || !isReady || (state == State::BETTING))
			{
				throw "-1";
			}
			return (std::to_string(PLAYER->bettingAmount[0]));
		}
		if(strcmp(param.keys[0], "getinsuranceamount") == 0)
		{
			time_t timeOut = time(0) + 5; // 5 second timeout time
			while( (!isReady) )
			{
				if(time(0) >= timeOut)
				{
					break;
				}
				SLEEP(0.1);
			}
			if(!running || !isReady || ((state == State::BETTING) || (state == State::INSURANCE)))
			{
				throw "-1";
			}
			return (std::to_string(PLAYER->insuranceAmount));
		}
		if(strcmp(param.keys[0], "getphase") == 0)
		{
			if(!running)
				return "-1";
			time_t timeOut = time(0) + 5; // 5 second timeout time
			while( (!isReady) )
			{
				if(time(0) >= timeOut)
				{
					break;
				}
				SLEEP(0.1);
			}
			return std::to_string(state);
		}
		if(strcmp(param.keys[0], "getturn") == 0)
		{
			time_t timeOut = time(0) + 5; // 5 second timeout time
			while( (!isReady) )
			{
				if(time(0) >= timeOut)
				{
					break;
				}
				SLEEP(0.1);
			}
			if(!running || !isReady || (state != State::HITTING))
			{
				throw "-1";
			}
			return (std::to_string(userTurnIndex));
		}
		if(strcmp(param.keys[0], "ack") == 0)
		{
			if(!running)
				return "-1";
			isReady = false;
			return "ACKNOWLEDGED";
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

	PLAYER->resetHand();
	dealer->resetHand();

	users.clear();	
	delete dealer;

	currentUsers = 0;
}

bool
BlackJack::isRunning()
{
	return running;
}

User *
BlackJack::addUser(User * user)
{
	BlackjackUser * blackjackUser = BlackjackUser::create(*user);
	delete user;
	users.push_back(blackjackUser);
	return blackjackUser;
}