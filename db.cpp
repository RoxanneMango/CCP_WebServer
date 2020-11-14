#include "socket.h"
#include "sleep.h"
#include "thread.h"
#include "database_server.h"

void saveTimer(DatabaseServer & db, const char * saveFile);

int
main(int argc, char ** argv)
{
	unsigned int id			=	0;
	unsigned int bufferSize	=	255;
	//
	const char * ip			= 	"127.0.0.1";
	unsigned short port		=	44901;
	//
	ListenSocket		listenSocket(ip, port);
	Socket		 		connectionSocket;
	DatabaseServer		database(id, listenSocket, connectionSocket, bufferSize);
		
	const char * loadFile = (argc == 2) ? argv[1] : "database.db";
	const char * saveFile = (argc == 3) ? argv[2] : "database_save.db";
	//
	printf("load file: %s\n", loadFile);
	printf("save file: %s\n", saveFile);
	//
	if(database.load(loadFile) < 0)
	{
		printf("Could not load from file.");
		return -1;
	}
	if(database.save(saveFile) < 0)
	{
		printf("Could not save to file.");
		return -1;
	}
	
	std::thread autoSave(saveTimer, std::ref(database), saveFile);
	
	printf("db listening on %s:%d . . .\n", ip, port);
	for(;;)
	{
		// Waiting for connection -> if connection, read from socket;
		database.listenClient();
		database.readSocket();
		database.processQuery();
		database.writeSocket();
		database.disconnectSocket();
	}
	return 0;
}

void
saveTimer(DatabaseServer & db, const char * saveFile)
{
	for(;;)
	{
		printf("Saving . . .\n");
		db.save(saveFile);
		printf("Save successful.\n");
		SLEEP(60);
	}
}