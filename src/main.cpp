#include "black_jack.h"
#include "online_database.h"
#include "web_server.h"
#include "socket.h"
#include "sleep.h"
#include "console.h"

//http://www.hoeoudbenikookalweer.nl:44900

// https://www.w3schools.com/js/js_cookies.asp

int
main(int argc, char ** argv)
{
//	const char * ipAddress	= 	"192.168.2.1";
	const char * ipAddress	= 	"127.0.0.1";
	unsigned short port		=	44900;
	
	int blackJack_id 				= 0;
	unsigned int numberOfCardDecks	= 3;

	Socket 				connectionSocket;
	ListenSocket	 	listenSocket(ipAddress, port);
	BlackJack 			blackJack(blackJack_id, numberOfCardDecks);

	unsigned int id_db			=	0;
	const char * ipAddress_db	= 	"127.0.0.1";
	unsigned short port_db		=	44901;
	unsigned int bufferSize_db	=	255;
	//
	Socket				connectionSocket_db(ipAddress_db, port_db);
	OnlineDatabase		database(id_db, connectionSocket_db, ipAddress_db, port_db, bufferSize_db);
	
	// All the dependency injection
	WebServer server(listenSocket, connectionSocket, "view/", "index.html");
	server.addDatabase(&database);
	server.addGame(&blackJack);
	
	unsigned int consoleBufferSize = 32;
	Console console(server, consoleBufferSize);

	for(;;)
	{
		// Waiting for connection -> if connection, read from socket;
		server.listenClient();

		server.readSocket();
		
//		server.printRequest(); printf("\n"); fflush(stdout);
		
		switch(server.checkRequestType())
		{
			case WebServer::Type::Get:
			{
				server.getHeader();
				server.getPage();
				break;
			}
			case WebServer::Type::Post:
			{
				server.getHeader();
				server.processParam();
				break;
			}
			default:
			{
				printf("Package type error.\n");
				break;
			}
		}
		server.writeSocket();
		server.disconnectSocket();
	}
	return 0;
}