#ifndef REG_CLI__H

#define REG_CLI__H


typedef struct  _RegisteredClient
{
	int id;
	char alias[30];

	int sockChannel;
	bool connected = false;
	MsgQueue msgQueue;

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