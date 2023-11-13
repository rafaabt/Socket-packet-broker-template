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


    int acceptConnection();
    void conn(const char *addr, unsigned int port); //!< Connects to the server listening on a specific address and port
    void closeSocket ();
    Packet streamSendPacket(const Packet &pSend, int channel); //!< Sends the packet, and waits for the response
    Packet streamSendPacket(const Packet &pSend);
    void streamSendPacketNoResp(const Packet &pSend, int channel); //!< Sends a packet, there's no response
    void streamSendPacketNoResp (const Packet &pSend);
    Packet streamRecvPacket (int channel);  //!< Receives a packet
    Packet streamRecvPacket ();
    int getServSockFd();

private:
    int sockServConn;   //!< server-side socket for connections
    int sockChannel;    //!< channel to exchange data (server and clients)
    struct sockaddr_in address;
    bool isServerSide;
};

#endif