// Server side C/C++ program to demonstrate Socket programming

#include <iostream>
#include <thread> 
#include "Server.h"

#define PORT 8080

using namespace std;


int main(int argc, char const* argv[])
{
	Server *serv = new Server(PORT);
	thread thConn(Server::LoopConnections, ref(serv));
	//thread thLoop(Server::LoopRequests,    ref(serv));

	thConn.join();
	//thLoop.join();

	delete serv;
	return 0;
}
