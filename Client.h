#ifndef CLI__H
#define CLI__H


#include <iostream>
#include <string>
#include <vector>
#include <arpa/inet.h>  // inet_pton
#include <cstring>      // memcpy
#include "Stream.h"
#include "Packet.h"


//! Client class  
/*!   
* This class models a simple client for socket programming. 
**/

class Client: public Stream
{
public:
    Client():Stream()
    {		
        packetId = 0;
    }

    ~Client()
    {}


    /**< A more detailed description */
    void conn(const char *addr, unsigned int port); //!< Connects to the server listening on a specific address and port
    void login (const char *alias);  //!< Logs to the server


    /*!  void insertPacket (const char *msg, size_t receiverId);
    \brief Sends a packet to the server. The packet will be inserted in the message queue (server side)
    @see Server.h
    @see Server::loop
    */
    void insertPacket (const char *msg, size_t receiverId);
    void removePacket (size_t pos);
    uint32_t getNumberOfPackets ();
    void downloadPackets ();
    void clearPackets ();
    void sendMsg (const char *msg);
    void sendMsgNoResp (const char *msg);
    void closeConn ();

    uint32_t getId() { return id;}
private:
    uint32_t id;
    size_t packetId;
};


#endif