#include "Client.h"



void Client::login (const char *alias)
{
    Packet pSend;

    pSend.header.senderId   = id;
    pSend.header.receiverId = -1;
    pSend.cmd = CMD_LOGIN;

    strcpy ((char*)pSend.buffer, alias);
    Packet rcv = streamSendPacket (pSend);

    id = rcv.field;

    //printf ("New assigned id is %d\n", id);
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

    //printf("[Resp] %s\n", (char*)pkt.buffer);
    return pkt.field;
}


void Client::downloadPackets ()
{
    uint32_t nPackets = getNumberOfPackets();

    Packet pSend;

    if (nPackets == 0)
    {
        printf ("No packets to download...\n");
        return;
    }

    pSend.cmd = CMD_DOWNLOAD_PACKETS;

    Packet pktResp = streamSendPacket (pSend);
    printf("[Resp] %s\n", (char*)pktResp.buffer);

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
    bzero (pSend.buffer, sizeof(void*)*BUFF_SIZE);
    Packet pkt = streamSendPacket (pSend);
    printf ("[Resp] %s\n", (char*)pkt.buffer);
    closeSocket();	
}