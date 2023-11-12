#ifndef STREAM__H
#define STREAM__H

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>  // inet_pton
#include <sys/socket.h>
#include <unistd.h>
#include "Packet.h"

using namespace std;


#define CHANNEL_FINISHED 0x2A


/*!   
* Stream class. Handles low-level socket file descriptors
* Opens sockets file descriptors and handles the send/recv to transfer packets from client(server) to server(client) 
**/


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
        isServerSide = false;
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
        isServerSide = true;
    }

    ~Stream()
    {
        if (isServerSide)
            close (sockServConn);
        close (sockChannel);
    }


    void conn(const char *addr, unsigned int port) //!< Connects to the server listening on a specific address and port
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


    void closeSocket ()
    {
        close(sockChannel);
    }


    Packet streamSendPacket(const Packet &pSend, int channel) //!< Sends the packet, and waits for the response
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

    void streamSendPacketNoResp(const Packet &pSend, int channel) //!< Sends a packet, there's no response
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

    Packet streamRecvPacket (int channel) //!< Receives a packet
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

    Packet streamRecvPacket ()
    {
        return streamRecvPacket(sockChannel);
    }

    int getServSockFd()
    {
        return sockServConn;
    }

    struct sockaddr_in address;

private:
    int sockServConn;   //!< server-side socket for connections
    int sockChannel;    //!< channel to exchange data (server and clients)
    bool isServerSide;
};

#endif