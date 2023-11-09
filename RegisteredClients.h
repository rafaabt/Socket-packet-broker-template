#ifndef REG_CLI__H
#define REG_CLI__H


#include <mutex>


extern mutex mtxLock;

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
		mtxLock.lock();
		msgQueue.insert(packet);
		mtxLock.unlock();
	}

	void removePacketById (ssize_t id)
	{
		mtxLock.unlock();
		msgQueue.removePacketById(id);
		mtxLock.unlock();
	}

	void listPackets ()
	{
		msgQueue.list();
	}

	void clearPackets()
	{
		mtxLock.lock();
		msgQueue.clearPackets();
		mtxLock.unlock();
	}

} RegisteredClient;


#endif