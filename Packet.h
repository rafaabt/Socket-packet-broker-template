#ifndef PACKET__H
#define PACKET__H

#include <iostream>
#include <vector>
#include <algorithm>

#define BUFF_SIZE   200
#define MAX_PACKETS 100


enum  // Client->Server commands
{	
	CMD_MSG,
	CMD_LOGIN,
	CMD_INSERT_PACKET,
	CMD_REMOVE_PACKET,
	CMD_CLEAR_PACKETS,
	CMD_LOGOFF,
	CMD_LIST_CLI,
	CMD_DOWNLOAD_PACKETS,
	CMD_GET_NUMBER_OF_PACKETS,
} Cmd;

extern const char *cmdStrings[20];

typedef struct _Header
{
	_Header()
	{
		valid     = false;
		encrypted = false;
	}
	bool valid;
	bool encrypted;
	uint32_t packetId;
	uint32_t senderId;
	uint32_t receiverId;
} PacketHeader;


/*!   
 *  A Packet is a unit of data transfer between a client and a server. 
 **/

typedef struct _Packet
{
	_Packet(){}

	PacketHeader header;
	uint32_t cmd;			  //!< Requested command
	uint32_t field;			  //!< General-purpose field
 	void *buffer[BUFF_SIZE];  //!< Generic information (e.g., string messages or structured data...)
} Packet;



/*!
	MsgQueue resides in the server-side. Each connected client keeps a MsgQueue, consisted of a vector o Packets
**/
typedef struct _MsgQueue
{
	std::vector<Packet> packets; //!< The packets inserted by each client

	 _MsgQueue()
	 {
	 	//nPackets = 0;
	 }

	void insert (const Packet &packet)
	{
		packets.push_back (packet);
		//nPackets++;
	}


	void removePacketById (size_t id)
	{
		for (std::vector<Packet>::iterator it = packets.begin(); it != packets.end(); it++)
		{
			if (it->header.packetId == id)
			{
				packets.erase(it);
				//nPackets--;
				break;
			}
		}
	}

	void clearPackets()
	{
		packets.clear();
		//nPackets = 0;
	}

	void list ()
	{
		uint32_t senderId = packets[0].header.senderId;

		//std::cout << "Client: " << senderId << " / " << "Packets: " << nPackets << std::endl;

		for (Packet pkt: packets)
			std::cout << (char*)pkt.buffer << std::endl;
		
		std::cout << std::endl;
	}
} MsgQueue;




#endif