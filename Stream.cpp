#include "Stream.h"


void St::Stream::conn(const char *addr, unsigned int port) //!< Connects to the server listening on a specific address and port
{
    int status;

    address.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, addr, &address.sin_addr) <= 0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(0);
    }

    if ((status = connect(clientSock, (struct sockaddr*)&address, sizeof(address))) < 0) 
    {
        printf("\nConnection Failed \n");
        exit(0);
    }
}


int St::Stream::acceptConnection()
{
    int newSock;
    socklen_t addrlen = sizeof(address);

    if ((newSock = accept(serverSock, (struct sockaddr*)&address, &addrlen)) < 0) 
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    return newSock;
}


void St::Stream::closeSocket ()
{
    if (!isServerSide)
        close(clientSock);
    close (clientSock);
}


Packet St::Stream::streamSendPacket(const Packet &pSend, int channel) //!< Sends the packet, and waits for the response
{
    ssize_t r = send(channel, (const void*)&pSend, sizeof(Packet), 0);

    if (r == -1)
    {
        printf("Error sending data\n");
        exit(0);
    }

    return streamRecvPacket(channel); // response
}


Packet St::Stream::streamSendPacket(const Packet &pSend)
{
    return streamSendPacket (pSend, clientSock);   
}


void St::Stream::streamSendPacketNoResp(const Packet &pSend, int channel) //!< Sends a packet, there's no response
{
    ssize_t r = send(channel, (const void*)&pSend, sizeof(Packet), 0);

    if (r == -1)
    {
        printf("Error sending data\n");
        exit(0);
    }
}


void St::Stream::streamSendPacketNoResp (const Packet &pSend)
{
    return St::Stream::streamSendPacketNoResp (pSend, clientSock);
}


Packet St::Stream::streamRecvPacket (int channel) //!< Receives a packet
{
    Packet pRecv;

    int bytesRecv = 0;
    pRecv.header.valid = false;

    if (channel == CHANNEL_FINISHED)
        return pRecv;
    
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


Packet St::Stream::streamRecvPacket ()
{
    return St::Stream::streamRecvPacket(clientSock);
}


int St::Stream::getServSockFd()
{
    return serverSock;
}