#ifndef REG_CLI__H

#define REG_CLI__H


/*!   
 *  RegisteredClient 
 * 	Manages the packet que for each client 
 **/

typedef struct  _RegisteredClient
{
	int id;
	char alias[30];

	int sockChannel;
	bool connected = false;
	MsgQueue msgQueue;	//!< The queue of messages inserted by the client

	void insertPacket (const Packet &packet)
	{
		msgQueue.insert(packet);
	}

	void removePacketById (ssize_t id)
	{
		msgQueue.removePacketById(id);
	}

	void listPackets ()
	{
		msgQueue.list();
	}

	void clearPackets()
	{
		msgQueue.clearPackets();
	}

} RegisteredClient;


#endif