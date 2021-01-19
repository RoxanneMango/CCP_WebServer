#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <time.h>

#include "param.h"
#include "file_io.h"
#include "game.h"
#include "blackjack_user.h"
#include "server.h"
#include "sleep.h"
#include "thread.h"

class WebServer : public Server
{
	char * RESPONSE = 0;
	//
	const char * OK_RESPONSE = "HTTP/1.0 200 OK\r\n";
	const char * FORBIDDEN_RESPONSE = "HTTP/1.0 403 forbidden\r\n";
	const char * TP_RESPONSE = "HTTP/1.0 418 I'm a teapot\r\n";

	const char * PREFIX = "view/";
	const char * OK_CODE = "OK";
	
	bool hasFavicon = false;
	
	unsigned int headerSize;
	char * header;

	FileIO fileIO;
	std::vector<Game *> games;

	unsigned int minNameLength = 2;
	unsigned int maxNameLength = 16;
	//
	unsigned int minUsernameLength = 4;
	unsigned int maxUsernameLength = 16;
	//
	unsigned int minPasswordLength = 8;
	unsigned int maxPasswordLength = 32;

	unsigned int maxUsers = 5;
	std::vector<User *> users;
	
	void clearBuffers();
	void resizeHeader();
	
	bool isPublic(char * fileName);
	
	void saveTimer();
	
	std::thread saveThread;
	
public:
	WebServer(Socket & serverSocket, Socket & clientSocket, Param & param, const char * viewDir, const char * defaultFile);
	~WebServer();

	enum Type { Post, Get, Put, Patch, Delete };

	Param & param;

	void setHeaderSize(unsigned int headerSize);

	void writeSocket();

	void printRequest();
	void printRequestFirstLine();
	//
	int checkRequestType();
	void getPage();
	//
	int getKeyAndValue();
	void processParam();
	
	void addGame(Game * game);
	void removeGame(int id);
	
	const char * registerUser(std::string name, std::string username, std::string password, bool isAdmin = false);
	const char * login(std::string token, std::string username, std::string password, std::string ip);
	bool logout(std::string token, std::string ip);
	bool verifyUser(std::string token, std::string ip);
};

#endif // WEB_SERVER_H