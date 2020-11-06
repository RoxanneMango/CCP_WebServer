#include "arpa_listen_socket.h"
#include "web_server.h"
#include "black_jack.h"
#include "online_database.h"

int
main(int argc, char ** argv)
{	
	const char * ipAddress	= 	"192.168.1.9";
//	const char * ipAddress	= 	"127.0.0.2";
	unsigned short port		=	44900;
	unsigned int paramSize 	= 	64;
	
	User user(0, "Player");
	User admin(1, "Dealer");
	
	int blackJack_id 				= 0;
	unsigned int numberOfCardDecks	= 3;
	
	ArpaSocket 			connectionSocket;
	ArpaListenSocket 	listenSocket(ipAddress, port);
	Param 				param(paramSize);
	BlackJack 			blackJack(blackJack_id, numberOfCardDecks);
	
	unsigned int id_db			=	0;
	const char * ipAddress_db	= 	"127.0.0.1";
	unsigned short port_db		=	44901;
	unsigned int bufferSize_db	=	255;
	//
	ArpaSocket 			connectionSocket_db(ipAddress_db, port_db);
	OnlineDatabase		database(id_db, connectionSocket_db, ipAddress_db, port_db, bufferSize_db);
	
	// All the dependency injection
	WebServer server(listenSocket, connectionSocket, param);

	// create root account
//	server.registerUser("root", "root", "midnightsnack", true);
	
	server.addDatabase(&database);
	server.addGame(&blackJack);
	
	for(;;)
	{
		// Waiting for connection -> if connection, read from socket;
		server.listenClient();
		server.readSocket();
		
		//server.printRequestFirstLine(); printf("\n"); fflush(stdout);
		
		switch(server.checkRequestType())
		{
			case WebServer::TYPE::GET:
			{
				server.getPage();
				break;
			}
			case WebServer::TYPE::POST:
			{
				//server.param.print();
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