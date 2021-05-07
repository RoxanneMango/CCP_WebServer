#include "web_server.h"

WebServer::WebServer( Socket & serverSocket, Socket & clientSocket, Param & param, const char * viewDir, const char * defaultFile) :
	Server(serverSocket, clientSocket),
	fileIO(viewDir, defaultFile),
	param(param),
	headers(128)
{	
	// default headerSize
	this->headerSize = 255;
	
	// allocate memory for the header with size headerSize
	this->header = (char *) calloc(headerSize, sizeof(char));
	
	RESPONSE = (char *) calloc(64, sizeof(char));
	strcpy(RESPONSE, "HTTP/1.0 200 OK\r\n");
	
	RouteFactory::load(routes, "src/routes.json");
	
	try
	{
		if(saveThread.joinable())
		{
			saveThread.detach();
		}
		saveThread = std::thread(&WebServer::saveTimer, this);
	}
	catch(std::exception const & exception)
	{
		printf("Exception: %s\n", exception.what());
		return;
	}
	
}

WebServer::~WebServer()
{
	// free all allocated memory for header
	free(header);
	free(RESPONSE);
}

void
WebServer::save()
{
	if(!databases[selectedDatabase]->isBusy)
	{
		databases[selectedDatabase]->save(users);
		printf("> Save successful\n");
	}
	else
	{
		printf("[!] Database is already saving!\n");
	}
}

void
WebServer::saveTimer()
{
	for(;;)
	{
		printf("save\n");
		save();
		SLEEP(60);
	}
}

void
WebServer::setHeaderSize(unsigned int headerSize)
{
	this->headerSize = headerSize;
	resizeHeader();
}

void
WebServer::resizeHeader()
{
	this->header = (char *) realloc(header, headerSize * sizeof(char));
}

void
WebServer::writeSocket()
{	
	int size = strlen(RESPONSE) + strlen(header) + buffer.size() + 1;
	char * x = (char *) calloc(size, sizeof(char));
	snprintf(x, size, "%s%s%s", RESPONSE, header, &buffer[0]);
	clientSocket.writeSocket(x, strlen(x));
	clearBuffers();
	free(x);
}

void
WebServer::clearBuffers()
{
	memset(receiveBuffer, 0, bufferSize);
	buffer.clear();
	
	memset(RESPONSE, 0, strlen(RESPONSE));
	memset(header, 0, headerSize);

	strcpy(RESPONSE, OK_RESPONSE);
	strcpy(header, "\r\n");
}

void
WebServer::printRequest(unsigned int len)
{
	if(!len)
	{
		len = strlen(receiveBuffer);
	}
	for(unsigned int i = 0; i < len; i++)
	{
		printf("%c", receiveBuffer[i]);
	}
}

void
WebServer::printRequestFirstLine()
{
	for(int i = 0; receiveBuffer[i] != 'H'; i++)
	{
		printf("%c", receiveBuffer[i]);
	}
}

int 
WebServer::checkRequestType()
{
	int type;
	if(!strncmp(receiveBuffer, "POST", 4))
	{
		type = Post;
	}
	else if(!strncmp(receiveBuffer, "GET", 3))
	{
		type = Get;
	}
	else if(!strncmp(receiveBuffer, "PUT", 3))
	{
		type = Put;
	}
	else if(!strncmp(receiveBuffer, "PATCH", 5))
	{
		type = Patch;
	}
	else if(!strncmp(receiveBuffer, "DELETE", 6))
	{
		type = Delete;
	}
	else
	{
		type = -1;
	}
	return type;
}

void
WebServer::getPage()
{
	// make pointer which points to start of receiveBuffer
	char * receiveBuffer_ptr = receiveBuffer;
	
	std::string filePath = std::string(PREFIX);
	std::string fileName;

	// skip through receiveBuffer until '/' is reached
	while(*receiveBuffer_ptr != '/')
	{
		receiveBuffer_ptr++;
	}
	// step over '/'
	receiveBuffer_ptr++;

	// step through receiveBuffer until a whitespace is reached
	while(*receiveBuffer_ptr != ' ')
	{
		fileName += *receiveBuffer_ptr;
		receiveBuffer_ptr++;
	}

	// append file name to default path
	filePath.append(fileName);
	
	// receiveBuffer_ptr is at end of file name; get to the start of 'css' file extension by taking 3 steps back
	// if file extension is 'css' (cascading style sheet), set content-type to text/css
	// else, use default settings
	if(!strncmp(receiveBuffer_ptr-3, "css", 3))
	{
		strcpy(header, "Content-Type: text/css\r\n\r\n");
	}
	
	// if server requests a file that is not favicon.ico, try to get file
	// if server requests favicon.ico, and hasFavicon is set to true, try to get file
	// requests for favicon.ico will be ignored if hasFavicon is false to avoid exception spam
	if( strcmp(fileName.c_str(), "favicon.ico") || (!(strcmp(fileName.c_str(), "favicon.ico")) && (hasFavicon)) )
	{
		// check if page is registered
		bool pageExists = false;
		bool isPublic = false;
		if(fileName.size())
		{
			for(unsigned int i = 0; i < routes.size(); ++i)
			{
				if(!strcmp(routes[i]->url.c_str(), fileName.c_str()) || !(strcmp(routes[i]->PATH.c_str(), fileName.c_str())))
				{
					pageExists = true;
					isPublic = routes[i]->isPublic;
					break;
				}
			}
		}
		else
		{
			for(unsigned int i = 0; i < routes.size(); ++i)
			{
				if(!strcmp(routes[i]->url.c_str(), "default"))
				{
					pageExists = true;
					isPublic = routes[i]->isPublic;
					break;
				}
			}
		}
		if(pageExists)
		{
			// if it is not a public page, verify user based on ip address
			// if the user is verified, serve page, otherwise give error 403 : forbidden
			if(isPublic)
			{
				// get page
				buffer.assign(fileIO.getFileContent(filePath.c_str()));
			}
			else
			{
				if(authenticate())
				{
					buffer.assign(fileIO.getFileContent(filePath.c_str()));
				}
				else
				{
					strcpy(RESPONSE, FORBIDDEN_RESPONSE);
				}
			}
		}
		else
		{
			strcpy(RESPONSE, NOT_FOUND_RESPONSE);
		}
	}
}

void
WebServer::processParam()
{
	try
	{
		// default return message
		buffer.assign(OK_CODE);
		
		// get params
		if(getKeyAndValue() < 0)
			throw "Please do not do that.";
		if( (param.keys.size() < 1) || (param.values.size() < 1) )
			throw "There were no params";
		if(strncmp(receiveBuffer, "POST /register", 14) == 0)
		{
			if(param.values.size() < 3)
			{
				throw "Not enough parameters for register command.";
			}
			const char * response = registerUser(param.values[1], param.values[2], param.values[3]);
			if(response != OK_CODE)
			{
				throw response;
			}
		}
		else if(strncmp(receiveBuffer, "POST /login", 11) == 0)
		{
			if(param.values.size() < 2)
			{
				throw "Not enough parameters for login command.";
			}
			const char * response = login(param.values[0], param.values[1], param.values[2], clientSocket.getIpAddress());
			if(strcmp(response, OK_CODE))
			{
				throw response;
			}
		}
		else if(strncmp(receiveBuffer, "POST /logout", 12) == 0)
		{
			if(param.values.size() < 1)
			{
				throw "Not enough parameters for logout command.";
			}
			if(!logout(param.values[0], clientSocket.getIpAddress()))
			{
				throw "Could not logout user";
			}
		}
		else if(strncmp(receiveBuffer, "POST /loggedIn", 14) == 0)
		{
			if(param.keys.size() < 1)
			{
				throw "Not enough parameters for loggedIn command.";
			}
			buffer.assign(std::to_string(verifyUser(param.values[0], clientSocket.getIpAddress())));
		}
		else if(strncmp(receiveBuffer, "POST /getName", 13) == 0)
		{
			for(unsigned int i = 0; i < users.size(); ++i)
			{
				if(users[i]->getIp() == clientSocket.getIpAddress())
				{
					if(users[i]->getToken() == param.values[0])
					{
						buffer.assign(users[i]->getName());
						return;
					}
				}
			}
			throw "no user logged in with this ip or token.";
		}
		else if(strncmp(receiveBuffer, "POST /getBalance", 16) == 0)
		{
			for(unsigned int i = 0; i < users.size(); ++i)
			{
				if(users[i]->getIp() == clientSocket.getIpAddress())
				{
					if(users[i]->getToken() == param.values[0])
					{
						time_t timeOut = time(NULL) + 5; // 5 second timeout time
						while(!users[i]->isReady)
						{
							if(time(NULL) >= timeOut)
							{
								break;
							}
							SLEEP(0.1);
						}
						buffer.assign(std::to_string(users[i]->getBalance()));
						return;
					}
				}
			}
			throw "no user logged in with this ip or token.";
		}
		else if(strncmp(receiveBuffer, "POST /addBalance", 16) == 0)
		{
			for(unsigned int i = 0; i < users.size(); ++i)
			{
				if(users[i]->getIp() == clientSocket.getIpAddress())
				{
					if(users[i]->getToken() == param.values[0])
					{
						time_t timeOut = time(NULL) + 5; // 5 second timeout time
						while(!users[i]->isReady)
						{
							if(time(NULL) >= timeOut)
							{
								break;
							}
							SLEEP(0.1);
						}
						if(!users[i]->isReady)
						{
							throw "user is not ready";
						}
						double balance = 0;
						if((balance = atof(param.values[1])) <= 0)
						{
							throw "cannot add balance : invalid balance";
						}
						users[i]->addBalance(balance);
						
						return;
					}
				}
			}
			throw "no user logged in with this ip or token.";
		}
		// admin game access
		else if(strncmp(receiveBuffer, "POST /game", 10) == 0)
		{
			
		}
		else if(strncmp(receiveBuffer, "POST /blackjack", 15) == 0)
		{
			// { command_key : command_value, token : <token> }
			if( (param.keys.size() < 1) || (param.values.size() < 1) )
				throw "Not enough keys or values for blackjack command.";

			// add ip address to param; Param class deallocates allocated resources automatically
			char * ip_key = (char *) calloc(7, sizeof(char));
			strcpy(ip_key, "ip_key");
			char * ip_value = (char *) calloc(strlen(&clientSocket.getIpAddress()[0]), sizeof(char));
			strcpy(ip_value, "ip_value");
			//
			param.keys.push_back(ip_key);
			param.values.push_back(ip_value);

			// first check for existence of blackjack game in the list of games
			for(Game * game : games)
			{
				if(game->name == "Black Jack")
				{
					// check if a user wants to join, and if they do verify their ip and token
					for(User * & loggedInUser : users)
					{
						// first verify ip address
						if(loggedInUser->getIp() == clientSocket.getIpAddress())
						{
							// then verify their token
							if(loggedInUser->getToken() == param.values[0])
							{
								if(!strcmp(param.keys[0], "join"))
								{
									if(game->isRunning())
										throw "GAME_ALREADY_IN_PROGRESS";
									if(game->currentUsers >= game->maxUsers)
										throw "MAX_USERS_REACHED";
									
									// check if user isn't already added to game
									if(game->currentUsers)
									{
										for(void * user : game->users)
										{
											if(loggedInUser->getID() == ((User *)user)->getID())
											{
												throw "USER_ALREADY_ADDED_TO_GAME";
											}
										}
									}
									loggedInUser = game->addUser(loggedInUser);
									game->currentUsers += 1;
									return;
								}
								else if(game->currentUsers)
								{
									if(loggedInUser->getID() == ((User *)(game->users[0]))->getID())
									{
										buffer.assign(game->input(param));
										return;
									}
								}
								else
								{
									throw "USER_NOT_REGISTERED_FOR_GAME";
								}
							}
							else
							{
								throw "INVALID_TOKEN";
							}
						}
					}
					throw "USER_NOT_FOUND";
				}
			}
			throw "GAME_NOT_FOUND";
		}
	}
	catch(const char * exception)
	{
		buffer.assign("Error : ");
		buffer.append(exception);
		return;
	}
}

int
WebServer::getKeyAndValue()
{
	return param.getKeyAndValue(receiveBuffer);
}

void
WebServer::addGame(Game * game)
{
	games.push_back(game);
}

void
WebServer::removeGame(int id)
{
	for(unsigned int i = 0; i < games.size(); ++i)
	{
		if(games[i]->id == id)
		{
			games.erase(games.begin()+i);
			return;
		}
	}
}

const char *
WebServer::login(std::string token, std::string username, std::string password, std::string ip)
{
	if(users.size() >= maxUsers)
	{
		for(User * user : users)
		{
			if(!strcmp(&user->getUsername()[0], &username[0]) && !(strcmp(&user->getPassword()[0], &password[0])))
			{
				user->login(token, ip);
				return "OK";
			}
		}
		return "server is full";
	}
	
	std::string q1 = "SELECT password FROM Players WHERE username = '" + username + "';";
	std::string q2 = "SELECT superUser FROM Players WHERE username = '" + username + "';";
	std::string q3 = "SELECT name FROM Players WHERE username = '" + username + "';";
	std::string q4 = "SELECT balance FROM Players WHERE username = '" + username + "';";

	double balance = atof(databases[selectedDatabase]->sendQuery(q4).c_str());

	printf("selected = %d\n", selectedDatabase);

	if(!strcmp(&databases[selectedDatabase]->sendQuery(q1)[0], &password[0]))
	{
		users.push_back(User::create(users.size(), databases[selectedDatabase]->sendQuery(q3), username, password, (atoi(&databases[selectedDatabase]->sendQuery(q2)[0]) == 1 ? true : false)));
		users[users.size()-1]->login(token, ip);
		users[users.size()-1]->setBalance(balance);
		return "OK";
	}
	return "could not match username to password";
}

const char *
WebServer::registerUser(std::string name, std::string username, std::string password, bool isAdmin)
{
	if(name.length() < minNameLength)
		return "This name is too short";
	else if(name.length() > maxNameLength)
		return "This name is too long";
	else if(username.length() < minUsernameLength)
		return "this username is too short";
	else if(username.length() > maxUsernameLength)
		return "this username is too long";
	else if(password.length() < minPasswordLength)
		return "this password is too short";
	else if(password.length() > maxPasswordLength)
		return "this password is too long";

	std::string q1 = "SELECT username FROM Players WHERE username = '" + username + "';";
	if(!strcmp(&databases[selectedDatabase]->sendQuery(q1)[0], &username[0]))
	{
		return "This username is already taken";
	}
	
	std::string q2 = "INSERT INTO Players superUser = " + std::string(isAdmin ? "1" : "0") + ", name = " + name + ", username = " + username + ", password = " + password + ";";
	if(!atoi(databases[selectedDatabase]->sendQuery(q2).c_str()))
	{
		printf("Registered user with \n\tname {%s};\n\tusername {%s};\n\tpassword {%s};\n", &name[0], &username[0], &password[0]);
		return "OK";
	}
	return "!OK";
}

bool
WebServer::logout(std::string token, std::string ip)
{
	for(unsigned int i = 0; i < users.size(); ++i)
	{
		if( (users[i]->getToken() == token) && (users[i]->getIp() == ip) )
		{
			users[i]->logout();
			databases[selectedDatabase]->save(*users[i]);
			delete users[i];
			users.erase(users.begin() + i);
			break;
		}
	}
	printf("users = %d\n", (int)users.size());
	return true;
}

bool
WebServer::verifyUser(std::string token, std::string ip)
{
	for(unsigned int i = 0; i < users.size(); ++i)
	{
		if( (users[i]->getToken() == token) && (users[i]->getIp() == ip) )
		{
			return true;
		}
	}
	return false;
}

void
WebServer::reload()
{
	RouteFactory::load(routes, "src/routes.json");
//	for(unsigned int i = 0; i < routes.size(); ++i)
//	{
//		printf("%s : %s : %d;\n", routes[i]->url.c_str(), routes[i]->PATH.c_str(), routes[i]->isPublic);
//	}
	printf("> Reload complete\n");
}

bool
WebServer::authenticate()
{
	for(unsigned int i = 0; i < users.size(); ++i)
	{
		if(users[i]->getIp() == clientSocket.getIpAddress())
		{
			return true;
		}
	}
	return false;
}