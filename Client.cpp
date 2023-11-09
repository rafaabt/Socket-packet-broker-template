#include "Client.h"


void Client::conn(const char *addr, unsigned int port)
{
	int status;

    address.sin_port = htons(port);

	// Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, addr, &address.sin_addr) <= 0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(0);
    }

	if ((status = connect(sockChannel, (struct sockaddr*)&address, sizeof(address))) < 0) 
    {
        printf("\nConnection Failed \n");
        exit(0);
    }
}


void Client::login (const char *alias)
{
	Packet pSend;

	pSend.header.senderId   = id;
	pSend.header.receiverId = -1;
	pSend.cmd = CMD_LOGIN;

	strcpy ((char*)pSend.buffer, alias);
	Packet rcv = streamSendPacket (pSend);
	printf ("[Resp] %s\n", (char*)rcv.buffer);
}


void Client::insertPacket (const char *msg, size_t receiverId)
{	
	Packet pSend;
	pSend.header.senderId   = id;
	pSend.header.receiverId = receiverId;
	pSend.header.packetId   = packetId++;
	pSend.cmd = CMD_INSERT_PACKET;

	strcpy ((char*)pSend.buffer, msg);
	Packet rcv = streamSendPacket (pSend);
	printf ("[Resp] %s\n", (char*)rcv.buffer);
}

void Client::removePacket (size_t packetId)
{	
	Packet pSend;
	pSend.header.senderId = id;
	pSend.cmd   = CMD_REMOVE_PACKET;
	pSend.field = packetId;


	Packet rcv = streamSendPacket (pSend);
	printf ("[Resp] %s\n", (char*)rcv.buffer);
}


uint32_t Client::getNumberOfPackets ()
{
	Packet pSend;
	pSend.header.senderId = id;
	pSend.cmd = CMD_GET_NUMBER_OF_PACKETS;

	Packet pkt = streamSendPacket (pSend);
	
	printf("[Resp] %s\n", (char*)pkt.buffer);
	return pkt.field;
}


void Client::downloadPackets ()
{
	uint32_t nPackets = getNumberOfPackets();

	Packet pSend;

	if (nPackets == 0)
		return;
	
	pSend.cmd = CMD_DOWNLOAD_PACKETS;

	streamSendPacketNoResp (pSend);
	
	printf("Total packets: %u\n", nPackets);

	printf("[Resp]: \n");

	for (size_t i = 0 ; i < nPackets; i++)
	{
		Packet pRecv = streamRecvPacket ();
		printf(" %s (packet %u) (client %u)\n", (char*)pRecv.buffer, pRecv.header.packetId, pRecv.header.senderId);
	}
}

void Client::clearPackets ()
{
	Packet pSend;
	pSend.header.senderId = id;
	//pSend.header.packetId = packetId++;
	pSend.cmd = CMD_CLEAR_PACKETS;

	Packet pkt = streamSendPacket (pSend);
	printf ("[Resp] %s\n", (char*)pkt.buffer);
}

void Client::sendMsg (const char *msg)
{	
	Packet pSend;
	pSend.header.senderId = id;
	//pSend.header.packetId = packetId++;
	pSend.cmd = CMD_MSG;

	strcpy ((char*)pSend.buffer, msg);
	Packet rcv = streamSendPacket (pSend);
	printf ("[Resp] %s\n", (char*)rcv.buffer);
}

void Client::sendMsgNoResp (const char *msg)
{	
	Packet pSend;
	pSend.header.senderId = id;
	//pSend.header.packetId = packetId++;
	pSend.cmd = CMD_MSG;

	strcpy ((char*)pSend.buffer, msg);
	streamSendPacketNoResp (pSend);
}


void Client::closeConn ()
{
	Packet pSend;
	pSend.cmd = CMD_LOGOFF;

	Packet pkt = streamSendPacket (pSend);
	printf ("[Resp] %s\n", (char*)pkt.buffer);
	
	close(sockChannel);
	//exit(0);
}