#include "web_server.h"

WebServer::WebServer( Socket & serverSocket, Socket & clientSocket, Param & param ) :
	Server(serverSocket, clientSocket),
	param(param)
{	
	// default headerSize
	this->headerSize = 255;
	
	// allocate memory for the header with size headerSize
	this->header = (char *) calloc(headerSize, sizeof(char));
	
	RESPONSE = (char *) calloc(64, sizeof(char));
	strcpy(RESPONSE, "HTTP/1.0 200 OK\r\n");
}

WebServer::~WebServer()
{
	// free all allocated memory for header
	free(header);
	free(RESPONSE);
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
WebServer::printRequest()
{
	for(unsigned int i = 0; i < strlen(receiveBuffer); i++)
	{
		printf("%c", receiveBuffer[i]);
	}
	printf("\n\n\n");
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
	if(strncmp(receiveBuffer, "POST", 4) == 0)
	{
		type = Post;
	}
	else if(strncmp(receiveBuffer, "GET", 3) == 0)
	{
		type = Get;
	}
	else if(strncmp(receiveBuffer, "PUT", 3) == 0)
	{
		type = Put;
	}
	else if(strncmp(receiveBuffer, "PATCH", 5) == 0)
	{
		type = Patch;
	}
	else if(strncmp(receiveBuffer, "DELETE", 6) == 0)
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
	// make pointer which points to start of receiveBuffer and initialize fileNameSize to 0
	char * receiveBuffer_ptr = receiveBuffer;
	unsigned int fileNameSize = 0;

	// skip through receiveBuffer until '/' is reached
	while(*receiveBuffer_ptr != '/')
	{
		receiveBuffer_ptr++;
	}
	// step over '/'
	receiveBuffer_ptr++;
	
	// step through receiveBuffer and increment fileNameSize with 1 until a whitespace is reached
	while(*receiveBuffer_ptr != ' ')
	{
		receiveBuffer_ptr++;
		fileNameSize += 1;
	}

	// create a new string called fileName with size of fileNameSize+1 (for null termination character)
	char * fileName = (char *) calloc(fileNameSize+1, sizeof(char));
	
	// receiveBuffer_ptr - fileNameSize rewinds the pointer to the start of the file name
	// copy contents of receiveBuffer_ptr into fileName, starting at beginning of fileName and ending at fileNameSize
	// pass fileName into fileIO class to retrieve contents of file

	strncpy(fileName, (receiveBuffer_ptr - fileNameSize), fileNameSize);
	
	
	char * filePath = (char *) calloc(fileNameSize + strlen(PREFIX) + 1, sizeof(char));
	strcpy(filePath, PREFIX);
	strcat(filePath, fileName);
	
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
	if( strcmp(fileName, "favicon.ico") || (!(strcmp(fileName, "favicon.ico")) && (hasFavicon)) )
	{
		// if it is not a public page, verify user based on ip address
		// if the user is verified, serve page, otherwise give error 403 : forbidden
		if(isPublic(fileName))
		{
			// get page
			buffer.assign(fileIO.getFileContent(filePath));
		}
		else
		{
			for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
			{
				if(loggedInUsers[i]->getIp() == clientSocket.getIpAddress())
				{
					buffer.assign(fileIO.getFileContent(filePath));
					free(fileName);
					return;
				}
			}
			strcpy(RESPONSE, FORBIDDEN_RESPONSE);
		}
	}
	// free the allocated memory of fileName
	free(fileName);
}

bool
WebServer::isPublic(char * fileName)
{
	// retrieve public pages from server, ideally . . .
	if(!(strlen(fileName))) return true;
	if(!(strcmp(fileName, "index.html"))) return true;
	if(!(strcmp(fileName, "login.html"))) return true;
	if(!(strcmp(fileName, "login.js"))) return true;
	if(!(strcmp(fileName, "loggedIn.js"))) return true;
	if(!(strcmp(fileName, "register.html"))) return true;
	if(!(strcmp(fileName, "register.js"))) return true;
	if(!(strcmp(fileName, "auth.js"))) return true;
	if(!(strcmp(fileName, "style.css"))) return true;
	
	return false;
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
			for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
			{
				if(loggedInUsers[i]->getIp() == clientSocket.getIpAddress())
				{
					if(loggedInUsers[i]->getToken() == param.values[0])
					{
						buffer.assign(loggedInUsers[i]->getName());
						return;
					}
				}
			}
			throw "no user logged in with this ip or token.";
		}
		else if(strncmp(receiveBuffer, "POST /getBalance", 16) == 0)
		{
			for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
			{
				if(loggedInUsers[i]->getIp() == clientSocket.getIpAddress())
				{
					if(loggedInUsers[i]->getToken() == param.values[0])
					{
						time_t timeOut = time(NULL) + 5; // 5 second timeout time
						while(!loggedInUsers[i]->isReady)
						{
							if(time(NULL) >= timeOut)
							{
								break;
							}
							SLEEP(0.1);
						}
						buffer.assign(std::to_string(loggedInUsers[i]->getBalance()));
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
					for(User * & loggedInUser : loggedInUsers)
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
		
//		sprintf(&buffer[0], "Error : %s", exception);
//		printf("Exception : %s\n", exception);
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
	if(loggedInUsers.size() >= maxUsers)
	{
		for(User * user : loggedInUsers)
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
	
	printf("selected = %d\n", selectedDatabase);

	if(!strcmp(&databases[selectedDatabase]->select(q1)[0], &password[0]))
	{
		loggedInUsers.push_back(new User(loggedInUsers.size(), databases[selectedDatabase]->select(q3), username, password, (atoi(&databases[selectedDatabase]->select(q2)[0]) == 1 ? true : false)));
		loggedInUsers[loggedInUsers.size()-1]->login(token, ip);
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
	if(!strcmp(&databases[selectedDatabase]->select(q1)[0], &username[0]))
	{
		return "This username is already taken";
	}
	
	std::string q2 = "INSERT INTO Players superUser = " + std::string(isAdmin ? "1" : "0") + ", name = " + name + ", username = " + username + ", password = " + password + ";";
	if(!databases[selectedDatabase]->insert(q2))
	{
		printf("Registered user with \n\tname {%s};\n\tusername {%s};\n\tpassword {%s};\n", &name[0], &username[0], &password[0]);
		return "OK";
	}
	return "!OK";
}

bool
WebServer::logout(std::string token, std::string ip)
{
	for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
	{
		if( (loggedInUsers[i]->getToken() == token) && (loggedInUsers[i]->getIp() == ip) )
		{
			loggedInUsers[i]->logout();
			delete loggedInUsers[i];
			loggedInUsers.erase(loggedInUsers.begin() + i);
			break;
		}
	}
	printf("loggedInUsers = %d\n", (int)loggedInUsers.size());
	return true;
}

bool
WebServer::verifyUser(std::string token, std::string ip)
{
	for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
	{
		if( (loggedInUsers[i]->getToken() == token) && (loggedInUsers[i]->getIp() == ip) )
		{
			return true;
		}
	}
	return false;
}