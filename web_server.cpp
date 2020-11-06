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
	snprintf(tranceiveBuffer, strlen(RESPONSE) + strlen(header) + strlen(buffer) + 1, "%s%s%s", RESPONSE, header, buffer);
	clientSocket.writeSocket(tranceiveBuffer, strlen(tranceiveBuffer));
	clearBuffers();
}

void
WebServer::clearBuffers()
{
	memset(receiveBuffer, 0, bufferSize);
	memset(tranceiveBuffer, 0, bufferSize);
	memset(buffer, 0, bufferSize);
	
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
		type = POST;
	}
	else if(strncmp(receiveBuffer, "GET", 3) == 0)
	{
		type = GET;
	}
	else if(strncmp(receiveBuffer, "PUT", 3) == 0)
	{
		type = PUT;
	}
	else if(strncmp(receiveBuffer, "PATCH", 5) == 0)
	{
		type = PATCH;
	}
	else if(strncmp(receiveBuffer, "DELETE", 6) == 0)
	{
		type = DELETE;
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
//		if(!strcmp(fileName, "home.html") || !strcmp(fileName, "user.html") || !strcmp(fileName, "black_jack.html") || !strcmp(fileName, "about.html"))
		{
			// get page
			fileIO.getFileContent(fileName, buffer);
		}
		else
		{
			for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
			{
				if(loggedInUsers[i].getIp() == clientSocket.getIpAddress())
				{
					fileIO.getFileContent(fileName, buffer);
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
		snprintf(buffer, 3, "%s", OK_CODE);
		
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
			if(response != OK_CODE)
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
			if(verifyUser(param.values[0], clientSocket.getIpAddress()))
			{
				snprintf(buffer, 2, "%d", 1);
			}
			else
			{
				snprintf(buffer, 2, "%d", 0);
			}
		}
		else if(strncmp(receiveBuffer, "POST /getName", 13) == 0)
		{
			for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
			{
				if(loggedInUsers[i].getIp() == clientSocket.getIpAddress())
				{
					if(loggedInUsers[i].getToken() == param.values[0])
					{
						sprintf(buffer, "%s", &loggedInUsers[i].getName()[0]);
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
			if( (param.keys.size() < 2) || (param.values.size() < 2) )
			{
				throw "Not enough keys or values for blackjack command.";
			}
			// add ip address to param; Param class deallocates allocated resources automatically
			char * ip_key = (char *) calloc(7, sizeof(char));
			strcpy(ip_key, "ip_key");
			char * ip_value = (char *) calloc(strlen(&clientSocket.getIpAddress()[0]), sizeof(char));
			strcpy(ip_value, "ip_value");
			//
			param.keys.push_back(ip_key);
			param.values.push_back(ip_value);

			// first check for existence of blackjack game in the list of games
			for(unsigned int i = 0; i < games.size(); ++i)
			{
				if(games[i]->name == "Black Jack")
				{
					// check if a user wants to join, and if they do verify their ip and token
					if(!(strcmp(param.keys[0], "blackjack")) && !(strcmp(param.values[0], "join")) && !(strcmp(param.keys[1], "token")))
					{
						if(games[i]->isRunning())
						{
							throw "GAME_ALREADY_IN_PROGRESS";
						}
						
						for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
						{
							// first verify ip address
							if(loggedInUsers[i].getIp() == clientSocket.getIpAddress())
							{
								// then verify their token
								if(loggedInUsers[i].getToken() == param.values[1])
								{
									// only if both the ip and the token have been verified, add user to game if it isn't at max users yet
									if(games[i]->currentUsers < games[i]->maxUsers)
									{
										// check if user isn't already added to game
										for(unsigned int i = 0; i < games[i]->users.size(); ++i)
										{
											if(loggedInUsers[i].getId() == games[i]->users[i]->getId())
											{
												throw "USER_ALREADY_ADDED_TO_GAME";
											}
										}
										games[i]->users.push_back(&loggedInUsers[i]);
										games[i]->currentUsers += 1;
										printf("added user to game!\n");
										
										return;
									}
									else
									{
										throw "MAX_USERS_REACHED";
									}
								}
							}
						}
						throw "USER_NOT_FOUND";
					}
					else
					{
						// return game output according to input
						sprintf(buffer, "%s", &games[i]->input(param)[0]);
						return;
					}
				}
			}
			throw "GAME_NOT_FOUND";
		}
	}
	catch(const char * exception)
	{
		sprintf(buffer, "Error : %s", exception);
		printf("Exception : %s\n", exception);
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
		return "server is full";
	}
	
	std::string q1 = "SELECT password FROM Players WHERE username = '" + username + "';";
	std::string q2 = "SELECT superUser FROM Players WHERE username = '" + username + "';";
	std::string q3 = "SELECT name FROM Players WHERE username = '" + username + "';";
	
	if(!strcmp(&databases[selectedDatabase]->select(q1)[0], &password[0]))
	{
		loggedInUsers.push_back(User(loggedInUsers.size(), databases[selectedDatabase]->select(q3), username, password, (atoi(&databases[selectedDatabase]->select(q2)[0]) == 1 ? true : false)));
		loggedInUsers[loggedInUsers.size()-1].login(token, ip);
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
		if( (loggedInUsers[i].getToken() == token) && (loggedInUsers[i].getIp() == ip) )
		{
			loggedInUsers[i].logout();
			loggedInUsers.erase(loggedInUsers.begin() + i);
			break;
		}
	}
	printf("loggedInUsers = %ld\n", loggedInUsers.size());
	return true;
}

bool
WebServer::verifyUser(std::string token, std::string ip)
{
	for(unsigned int i = 0; i < loggedInUsers.size(); ++i)
	{
		if( (loggedInUsers[i].getToken() == token) && (loggedInUsers[i].getIp() == ip) )
		{
			return true;
		}
	}
	return false;
}