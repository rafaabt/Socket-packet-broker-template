#ifndef CLI__H
#define CLI__H


#include <iostream>
#include <string>
#include <vector>
#include <cstring>    
#include <functional>
#include "Stream.h"
#include "Packet.h"



    struct UserData
    {
        int id;
        int data;
    };
    
//typedef function<void(void *p)> callback_t2;
using callback_t2 = std::function<void(void *p)>; // equivalente

typedef std::function<void(void)> callback_t1;
typedef std::function<void(int arg)> callback_t3;


typedef struct _CbHandle
{
    callback_t2 mConnect;
    callback_t1 mSend;
    callback_t1 mRecv;
} CbHandle;

//! Client class  
/*!   
* This class models a simple client for socket programming. 
**/

class Client: public St::Stream
{
public:
    Client():St::Stream()
    {		
        packetId = 0;
    }


    Client(CbHandle && cb):St::Stream()
    {
        mSend = cb.mSend;
        mRecv = cb.mRecv;
        mConnect = cb.mConnect;
    }

    callback_t2 mConnect;
    callback_t1 mSend;
    callback_t1 mRecv;

    ~Client()
    {}




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
    uint32_t getId() { return id; }

    int xxx = 42;
private:
    uint32_t id;
    size_t packetId;
};


#endif