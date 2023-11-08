#ifndef SERV__H
#define SERV__H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <mutex>
#include <cstring> // memcpy
#include <sys/wait.h>
#include "Packet.h"
#include "Stream.h"
#include "RegisteredClients.h"

#define MAX_ON_CLIENTS 100

using namespace std;

extern mutex mtxLock;


//! Server class  
/*!   
 * This class models a simple server for socket programming. 
 **/


class Server: public Stream
{
public:
	Server (ssize_t port):Stream(port)
	{
		nOnClients = 0;
		registerClients.clear();
	}


	/*
		LoopConnections(): A thread to accept new connections
	*/

	static void LoopConnections (Server *serv)
	{
		socklen_t addrlen = sizeof(serv->address);

		while (1)
		{
			printf("Server awaiting connection\n");

			int newSock;

			if ((newSock = accept(serv->sockServConn, (struct sockaddr*)&serv->address, &addrlen)) < 0) 
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//mtxLock.lock();

			RegisteredClient newClient;
			newClient.id          = serv->nOnClients;
			newClient.sockChannel = newSock;

			serv->registerClients.push_back(newClient);

			serv->loop (serv->registerClients[serv->nOnClients]);
			serv->nOnClients++;
			//printf("Registered new client (total %u)\n", serv->nOnClients);
			//mtxLock.unlock();
		}
	} 


	static void LoopRequests (Server *serv)
	{
		while (true)
		{
			vector<RegisteredClient>::iterator it;

			for (it = serv->registerClients.begin(); it != serv->registerClients.end(); it++)
			{
				serv->loop (*it);
			}
		}
	}

/*!  void loop (RegisteredClient &client);
    \brief Creates a child process to wait for client requests
*/

	void loop (RegisteredClient &client);
	void sendMsgNoResp (const char *msg, RegisteredClient& client);
	Packet recvPacket (RegisteredClient& client);
	void unregisterClient (RegisteredClient &client);
		

private:
	vector<RegisteredClient> registerClients;
	uint32_t nOnClients;
};


#endif