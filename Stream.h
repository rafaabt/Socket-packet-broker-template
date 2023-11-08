#ifndef STREAM__H
#define STREAM__H

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Packet.h"

using namespace std;


/*
	Opens sockets file descriptors and handles the send/recv to transfer packets from client(server) to server(client) 
*/


class Stream
{
public:
	Stream () // Client-side socket
	{
		if ((sockChannel = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		{
			perror("socket failed");
			exit(EXIT_FAILURE);
		}

		address.sin_family = AF_INET;
	}


	Stream (ssize_t port) // server-side socket
	{
		if ((sockServConn = socket(AF_INET, SOCK_STREAM, 0)) < 0)  // if this is the server-side, 
		{														   // then sockChannel is set after accept
			perror("socket failed");
			exit(EXIT_FAILURE);
		}
		
		int opt = 1;

		// Forcefully attaching socket to the port 8080
		if (setsockopt(sockServConn, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

		address.sin_family      = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port        = htons(port);

		if (bind(sockServConn, (struct sockaddr*)&address, sizeof(address)) < 0) 
		{
			perror("bind failed");
			exit(EXIT_FAILURE);
		}
		if (listen(sockServConn, 3) < 0) 
		{
			perror("listen");
			exit(EXIT_FAILURE);
		}
	}


	Packet streamSendPacket(const Packet &pSend, int channel) // sends the packet, and waits for the response
	{
		ssize_t r = send(channel, (const void*)&pSend, sizeof(Packet), 0);

		if (r == -1)
		{
			printf("Error sending data\n");
			exit(0);
		}

	    return streamRecvPacket(channel); // response
	}

	Packet streamSendPacket(const Packet &pSend)
	{
		return streamSendPacket (pSend, sockChannel);	
	}

	void streamSendPacketNoResp(const Packet &pSend, int channel) // juset sends a packet, there's no response
	{
		ssize_t r = send(channel, (const void*)&pSend, sizeof(Packet), 0);

		if (r == -1)
		{
			printf("Error sending data\n");
			exit(0);
		}
	}

	void streamSendPacketNoResp (const Packet &pSend)
	{
		return streamSendPacketNoResp (pSend, sockChannel);
	}

	Packet streamRecvPacket (int channel) // receives a packet
	{
		Packet pRecv;

		int bytesRecv = 0;
		pRecv.header.valid = false;

		while (bytesRecv < sizeof(Packet))
		{
			int r = read(channel, (void*)&pRecv, sizeof(Packet));
		
			if (r < 0) // returns with pRecv.header.valid = false
				return pRecv; 

			bytesRecv += r;
		}

		pRecv.header.valid = true;
		return pRecv;
	}

	Packet streamRecvPacket ()
	{
		return streamRecvPacket(sockChannel);
	}


	int sockServConn;   // server-side socket for connections
	int sockChannel;    // channel to exchange data (server and clients)
	struct sockaddr_in address;
};

#endif