#include "black_jack.h"

BlackJack::BlackJack(int id, unsigned int numOfDecks) : 
	Game(id), numOfDecks(numOfDecks)
{
	name = "Black Jack";
	minUsers = 1;
	currentUsers = 0;
	maxUsers = 1;
	running = false;
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
		if(((BlackjackUser *)(users[i]))->hand[0].size() == 0) return "!OK";

		hands.append(((BlackjackUser *)(users[i]))->getName());
		hands.append(":");
		
		for(unsigned int k = 0; k < (1 + (((BlackjackUser *)(users[i]))->hand[1].size() > 0)); ++k)
		{
			for(unsigned int j = 0; j < ((BlackjackUser *)(users[i]))->hand[k].size(); ++j)
			{
				int pv = ((BlackjackUser *)(users[i]))->hand[k][j].getPictureValue();
				int sv = ((BlackjackUser *)(users[i]))->hand[k][j].getSuitValue();

				// Second card dealt is face down -- hidden
				if(((BlackjackUser *)(users[i]))->hand[k][j].isHidden())
				{
					hands.append(std::string("hidden"));
					continue;
				}
				
				if(pv==0) hands.append(std::to_string(((BlackjackUser *)(users[i]))->hand[k][j].getValue()));
				else hands.append(pv==1?"J":pv==2?"Q":pv==3?"K":"A");
				hands.append(",");
				hands.append(sv==0?"H":sv==1?"C":sv==2?"D":"S");
				hands.append(",");
				hands.append(std::to_string(((BlackjackUser *)(users[i]))->hand[k][j].getValue()));
				if(j+1 != ((BlackjackUser *)(users[i]))->hand[k].size()) hands.append(":");
			}
			hands.append(";");
		}
	}
	printf("hands: %s\n", &hands[0]);
	return hands;
}

int
BlackJack::getSettlement()
{
	// -1	not settled
	//
	//	0	loss
	//	1	win
	//	2	draw
	//	3	loss loss
	//	4	draw loss
	//	5	loss draw
	//	6	draw draw
	//	7	win loss
	//	8	loss win
	//	9	win win
	
	if(!((BlackjackUser *)(users[0]))->hand[1].size())
	{
		if( isLose[0] && !isWon[0] )
			return 0;
		if( !isLose[0] && isWon[0] )
			return 1;
		return 2;
	}
	else
	{
		if( isLose[0] && isLose[1] && !isWon[0] && !isWon[1] )
			return 3;
		if( isLose[0] && isWon[0] && isLose[1] && !isWon[1] )
			return 4;
		if( isLose[0] && !isWon[0] && isLose[1] && isWon[1] )
			return 5;
		if( isLose[0] && isLose[1] && isWon[0] && isWon[1] )
			return 6;
		if( !isLose[0] && isWon[0] && !isWon[1] && isLose[1] )
			return 7;
		if( isLose[0] && !isWon[0] && isWon[1] && !isLose[1] )
			return 8;
	}
	return 9;
//	else( (!isLose[0] && !isLose[1]) && (isWon[0] && isWon[1]) )
}

void
BlackJack::init()
{
	printf("init() . . .\n");
	
	for(unsigned int i = 0; i <= numOfDecks; ++i)
	{
		cardDecks.push_back(DeckOfCards());
	}

	dealer = new Dealer(&cardDecks, &this->numOfDecks);
	users.push_back(dealer);
	
	dealer->shuffle();

	running = true;
	//
	isNatural = false;
	isDoubleDown = false;
	isSplit = false;
	isSurrender = false;
	isWon[0] = false;
	isWon[1] = false;
	isLose[0] = false;
	isLose[1] = false;
	isDone = false;
	isHit = false;
	isStand = false;
	//
	canSplit = false;
	canInsurance = false;

	((BlackjackUser *)(users[0]))->resetHand();
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
	userTurnIndex = 0;
	isReady = true;
}

void
BlackJack::bettingPhase()
{
	if(!isReady)
	{
		if(bettingAmount[0] > 0.00)
		{
			printf("Bet placed: %.2f\n", bettingAmount[0]); fflush(stdout);
			((User *)(users[0]))->subtractBalance(bettingAmount[0]);
			printf("New balance: %.2f\n", ((BlackjackUser *)(users[0]))->getBalance());
			isDone = true;
		}
		isReady = true;
	}
	if(isDone)
	{
		state = State::DEALING;
		((User *)(users[0]))->isReady = true;
		isReady = false;
	}
}

void
BlackJack::dealingPhase()
{
	
	if(!isReady)
	{
		printf("Dealing phase . . .\n");
		
		dealer->deal(&users);
		for(unsigned int i = 0; i < users.size(); ++i)
		{
			for(unsigned int j = 0; j < ((BlackjackUser *)(users[i]))->hand[0].size(); ++j)
			{
				((BlackjackUser *)(users[i]))->hand[0][j].print();
				printf("\n");
			}
			for(unsigned int j = 0; j < ((BlackjackUser *)(users[i]))->hand[0].size(); ++j)
			{
				((BlackjackUser *)(users[i]))->hand[0][j].print();
				printf("\n");
			}
			printf("\n");
		}

		canSplit = ((BlackjackUser *)(users[0]))->hand[0][0] == ((BlackjackUser *)(users[0]))->hand[0][1];
		canInsurance = (dealer->hand[0][0].getValue() == 11);
		isNatural = ((((BlackjackUser *)(users[0]))->hand[0][0].getValue() + ((BlackjackUser *)(users[0]))->hand[0][1].getValue()) == 21);
		
		isDone = (dealer->hand[0][0].getValue() != 11);
		isReady = true;
	}
	if(isNatural)
	{
		isDone = false;
		isReady = false;
		state = State::SETTLEMENT;
	}
	else if(canInsurance)
	{
		printf("INSURANCE!\n");
		isDone = false;
		state = State::INSURANCE;
	}
	else if(isDone)
	{	
		isDone = false;
		if(canSplit)
		{
			printf("SPLITTING!\n");
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
			((User *)(users[0]))->isReady = false;
			state = State::SETTLEMENT;
		}
		else if(canSplit)
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
		if(insuranceAmount > 0.00)
		{
			isDone = true;
			isReady = false;
			printf("Insurance Bet placed: %.2f\n", insuranceAmount); fflush(stdout);
			((User *)(users[0]))->subtractBalance(insuranceAmount);
			((User *)(users[0]))->isReady = true;
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
	if(isSplit)
	{
		if(((User *)(users[0]))->getBalance() >= bettingAmount[0])
		{
			bettingAmount[1] = bettingAmount[0];
			((User *)(users[0]))->subtractBalance(bettingAmount[1]);
		}
		dealer->split(((BlackjackUser *)(users[0])));
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
	if(isDoubleDown)
	{
		((User *)(users[0]))->subtractBalance(bettingAmount[userTurnIndex]);
		bettingAmount[userTurnIndex] *= 2;
		dealer->dealCard(((BlackjackUser *)(users[0])), userTurnIndex);
		userTurnIndex += 1;
		isDone = (userTurnIndex > (((BlackjackUser *)(users[0]))->hand[1].size() ? 1 : 0));
	}
	else if(isSurrender)
	{
		isSurrender = false;
		((User *)(users[0]))->addBalance(bettingAmount[userTurnIndex] / 2);
		isLose[userTurnIndex] = true;
		userTurnIndex += 1;
		isDone = (userTurnIndex > (((BlackjackUser *)(users[0]))->hand[1].size() ? 1 : 0));
	}
	else if(isHit)
	{
		isHit = false;
		if((userTurnIndex == 0) || (userTurnIndex == 1 && ((BlackjackUser *)(users[0]))->hand[1].size()))
		{
			dealer->dealCard(((BlackjackUser *)(users[0])), userTurnIndex);
			int sum = 0;
			for(Card card : ((BlackjackUser *)(users[0]))->hand[userTurnIndex])
			{
				sum += card.getValue();
			}
			if(sum > 21)
			{
				bool canLowerAce = false;
				for(Card & card : ((BlackjackUser *)(users[0]))->hand[userTurnIndex])
				{
					if(card.getPicture() == 'A')
					{
						if(card.getValue() == 11)
						{
							card.setValue(1);
							canLowerAce = true;
							break;
						}
					}
				}
				if(!canLowerAce)
				{
					printf("userturnindex = %d\n", userTurnIndex);
					isLose[userTurnIndex] = true;
					userTurnIndex += 1;
					isDone = (userTurnIndex > (((BlackjackUser *)(users[0]))->hand[1].size() ? 1 : 0));
				}
			}
		}
	}
	else if(isStand)
	{
		isStand = false;
		if((userTurnIndex == 0) && ((BlackjackUser *)(users[0]))->hand[1].size())
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
		if(!isLose[0] || !isLose[1])
		{
			int sum = dealer->hand[0][0].getValue() + dealer->hand[0][1].getValue();
			while(sum < 14)
			{			
				dealer->dealCard((BlackjackUser *)(dealer), 0);
				sum += dealer->hand[0][dealer->hand[0].size()-1].getValue();
			}
		}
		((User *)(users[0]))->isReady = false;
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

		int dealerSum = 0;
		int userSum1 = 0;
		int userSum2 = 0;

		for(Card card : ((BlackjackUser *)(users[0]))->hand[0])
		{
			userSum1 += card.getValue();
		}
		if(((BlackjackUser *)(users[0]))->hand[1].size())
		{
			for(Card card : ((BlackjackUser *)(users[0]))->hand[1])
			{
				userSum2 += card.getValue();
			}
		}
		
		for(Card card : dealer->hand[0])
		{
			dealerSum += card.getValue();
		}

		if((dealerSum == 21) && (dealer->hand[0].size() == 2) && (insuranceAmount > 0.00))
		{
			((User *)(users[0]))->addBalance(insuranceAmount * 2);
			isLose[0] = true;
			isLose[1] = true;
		}
		
		if(!isLose[0])
		{
			if(dealerSum == userSum1) // DRAW
			{
				isWon[0] = true;
				isLose[0] = true;
				((User *)(users[0]))->addBalance(bettingAmount[0] * (1 + isDoubleDown));
			}
			else if(isNatural)		// NATURAL
			{
				isWon[0] = true;
				((User *)(users[0]))->addBalance(bettingAmount[0] * 1.5);
			}
			else if((userSum1 > dealerSum) && (userSum1 <= 21)) // WIN
			{
				isWon[0] = true;
				((User *)(users[0]))->addBalance(bettingAmount[0] * (userSum1 == 21 ? 3 : 2));
			}
			else if(dealerSum > 21)
			{
				isWon[0] = true;
				((User *)(users[0]))->addBalance(bettingAmount[0] * (userSum1 == 21 ? 3 : 2));
			}
			else
			{
				isLose[0] = true;
			}
		}
		if(!isLose[1] && userSum2)
		{
			if(dealerSum == userSum2) // DRAW
			{
				isWon[1] = true;
				isLose[1] = true;
				((User *)(users[0]))->addBalance(bettingAmount[1] / (1 + isDoubleDown));
			}
			else if((userSum2 > dealerSum) && (userSum2 <= 21))	// WIN
			{
				isWon[1] = true;
				((User *)(users[0]))->addBalance(bettingAmount[1] * (userSum1 == 21 ? 3 : 2));
			}
			else if(dealerSum > 21)
			{
				isWon[1] = true;
				((User *)(users[0]))->addBalance(bettingAmount[0] * (userSum1 == 21 ? 3 : 2));
			}
			else
			{
				isLose[1] = true;
			}
		}
		isReady = true;
		((User *)(users[0]))->isReady = true;
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
			if(atof(param.values[1]) > ((BlackjackUser *)(users[0]))->getBalance())
				throw "NOT_ENOUGH_BALANCE";
			((BlackjackUser *)(users[0]))->isReady = false;
			bettingAmount[0] = atof(param.values[1]);
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
			if(atof(param.values[1]) > (bettingAmount[0] / 2))
				throw "INSURANCE_AMOUNT_TOO_BIG";
			((BlackjackUser *)(users[0]))->isReady = false;
			insuranceAmount = atof(param.values[1]);
			isReady = false;
			return "OK";
		}
		if(strcmp(param.keys[0], "split") == 0)
		{
			if( (param.keys.size() < 2) || (param.values.size() < 2) )
				throw "NOT_ENOUGH_PARAMS";
			if(!running || state != State::SPLITTING)
				throw "COULD_NOT_SPLIT_HAND";
			if(((BlackjackUser *)(users[0]))->getBalance() < bettingAmount[0])
				throw "NOT_ENOUGH_BALANCE_FOR_SPLIT";
			isSplit = true;
			isReady = false;
			return "OK";
		}
		if(strcmp(param.keys[0], "hit") == 0)
		{
			if(!running || state != State::HITTING)
				throw "COULD_NOT_HIT_CARD";
			if(isLose[0] && isLose[1])
				throw "CANNOT_HIT_WHEN_LOSE";
			if(isHit)
				throw "ALREADY_HIT";
			isHit = true;
			isReady = false;
			return "HIT";
		}
		if(strcmp(param.keys[0], "stand") == 0)
		{
			if(!running || state != State::HITTING)
				throw "COULD_NOT_STAND";
			if(isStand)
				throw "ALREADY_STANDING";
			isStand = true;
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
			isSurrender = true;
			isReady = false;
			return "SURRENDER";
		}
		if(strcmp(param.keys[0], "doubledown") == 0)
		{
			if(!running)
				throw "GAME_NOT_RUNNING";
			if(state != State::HITTING)
				throw "COULD_NOT_DOUBLE_DOWN";
			if(isDoubleDown)
				throw "ALREAD_DOUBLE_DOWN";
			if(((BlackjackUser *)(users[0]))->getBalance() < bettingAmount[0])
				throw "NOT_ENOUGH_BALANCE_FOR_DOUBLE_DOWN";
			isDoubleDown = true;
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
			return std::to_string(getSettlement());
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
			return (std::to_string(bettingAmount[0]));
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
			return (std::to_string(insuranceAmount));
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
	((BlackjackUser *)(users[0]))->resetHand();
	dealer->resetHand();

	users.clear();
	bets.clear();
	
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
	BlackjackUser * blackjackUser = new BlackjackUser(*user);
	delete user;
	users.push_back(blackjackUser);
	return blackjackUser;
}