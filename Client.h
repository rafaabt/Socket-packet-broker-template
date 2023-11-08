#ifndef CLI__H
#define CLI__H


#include <iostream>
#include <string>
#include <vector>
#include <arpa/inet.h>  // inet_pton
#include <cstring>      // memcpy
#include "Stream.h"
#include "Packet.h"


class Client: public Stream
{
public:
	Client(unsigned int id_):Stream()
	{
		id = id_;
		packetId = 0;
	}

	void conn(const char *addr, unsigned int port);
	void login (const char *alias);
	void insertPacket (const char *msg, size_t receiverId);
	void removePacket (size_t pos);
	void downloadPackets ();
	void clearPackets ();
	void sendMsg (const char *msg);
	void sendMsgNoResp (const char *msg);
	void closeConn ();

private:
	uint32_t id;
	//string alias;
	size_t packetId;

};


#endif