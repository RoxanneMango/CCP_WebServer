#ifndef CONSOLE_H
#define CONSOLE_H

#include "stdio.h"
#include "memory.h"

#include "web_server.h"
#include "thread.h"

class Console
{
private:
	std::thread thread;
	WebServer & webServer;

	char * buffer;
	unsigned int bufferSize = 0;
	
	void process();
	
public:
	Console(WebServer & webServer, unsigned int bufferSize);
	~Console();
	
	void run();
};

#endif // CONSOLE_H