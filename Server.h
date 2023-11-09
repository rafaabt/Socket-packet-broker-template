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
#define FINISHED       0x2A

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

	~Server ()
	{
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

			RegisteredClient newClient;
		
			newClient.sockChannel = newSock;

			mtxLock.lock();
			serv->registerClients.push_back(newClient);
			mtxLock.unlock();
		}
	} 


	static void LoopRequests (Server *serv)
	{
		while (true)
		{
			vector<RegisteredClient>::iterator it;
			size_t nCli;

			mtxLock.lock();
			nCli = serv->registerClients.size();
			mtxLock.unlock();

			for (int i = 0; i < nCli; i++)
			{
				mtxLock.lock();
				serv->loop (serv->registerClients[i]);
				mtxLock.unlock();
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
	map<uint32_t, RegisteredClient*> mapClient;
	uint32_t nOnClients;
};


#endif