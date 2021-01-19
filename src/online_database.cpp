#include "online_database.h"

OnlineDatabase::OnlineDatabase(unsigned int id, Socket & socket, const char * ip, unsigned short port, unsigned int bufferSize) : 
	socket(socket),
	ip(ip),
	port(port),
	bufferSize(bufferSize)
{
	this->id = id;
	receiveBuffer = (char *) calloc(bufferSize, sizeof(char));
	tranceiveBuffer = (char *) calloc(bufferSize, sizeof(char));
}

OnlineDatabase::~OnlineDatabase()
{
	free(receiveBuffer);
	free(tranceiveBuffer);
}

void
OnlineDatabase::clearBuffers()
{
	memset(receiveBuffer, 0, bufferSize);
	memset(tranceiveBuffer, 0, bufferSize);
}

std::string
OnlineDatabase::sendQuery(std::string query)
{
	clearBuffers();
	
	socket.connectSocket();
	socket.writeSocket(&query[0] , strlen(&query[0])+1);
	socket.readSocket(receiveBuffer, bufferSize);
	socket.disconnectSocket();

	return std::string(receiveBuffer);
}

std::string 
OnlineDatabase::save(std::vector<User *> & users)
{
	for(User * user : users)
	{
		// trim off 4 extra zeroes at end
		std::string tmp = std::to_string(user->getBalance());
		std::string balance = tmp.substr(0, tmp.length()-4);
		
		std::string q = "UPDATE Players balance = " + balance + "; WHERE username = '" + user->getUsername() + "';";
		sendQuery(q);
	}	

	return "SAVED";
}

std::string 
OnlineDatabase::save(User & user)
{
	// trim off 4 extra zeroes at end
	std::string tmp = std::to_string(user.getBalance());
	std::string balance = tmp.substr(0, tmp.length()-4);
	
	std::string q = "UPDATE Players balance = " + balance + "; WHERE username = '" + user.getUsername() + "';";
	sendQuery(q);
	
	return "SAVED";
}