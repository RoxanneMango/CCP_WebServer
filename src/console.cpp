#include "console.h"

Console::Console(WebServer & webServer, unsigned int bufferSize) : webServer(webServer), bufferSize(bufferSize)
{
	buffer = (char *) calloc(bufferSize + 1, sizeof(char));
	try
	{
		if(thread.joinable())
		{
			thread.detach();
		}
		thread = std::thread(&Console::run, this);
	}
	catch(std::exception const & exception)
	{
		printf("Exception: %s\n", exception.what());
		exit(-1);
	}
}

Console::~Console()
{
	free(buffer);
}

void
Console::run()
{
	for(;;)
	{
		if(fgets(buffer, bufferSize, stdin) != NULL)
		{
			if(buffer[strlen(buffer)-1] != '\n')
			{
				for (int c; (c = getchar()) != EOF && c != '\n';);
				printf("\n");
			}
			process();
			memset(buffer, 0, bufferSize);
		}
		else
		{
			printf("Error : fgets function returned NULL");
		}
	}
}

void
Console::process()
{
	// trim newline
	char command[strlen(buffer)-1] = { 0 };
	strncpy(command, buffer, strlen(buffer)-1);

	if(!strcmp(command, "reload"))
	{
		webServer.reload();
	}
	else if(!strcmp(command, "save"))
	{
		webServer.save();
	}
	else if(!strcmp(command, "help"))
	{
		printf("> I am sorry but you are on your own\n");
	}
	else
	{
		printf("[!] Unknown command\n");
	}
}