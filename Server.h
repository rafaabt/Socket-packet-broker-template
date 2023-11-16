#ifndef SERV__H
#define SERV__H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <mutex>
#include <cstring> // memcpy
#include <sys/wait.h>
#include "Packet.h"
#include "Stream.h"
#include "RegisteredClients.h"


#define MAX_ON_CLIENTS 100
#define SLEEP_MS(MS)   usleep(MS*1000);


extern std::mutex mtxLock;
extern int lastAcceptedChannel;
extern uint32_t glNewIntRequest;


//! Server class  
/*!   
* This class models a simple server for socket programming. 
**/


class Server: public St::Stream
{
public:
    Server (ssize_t port):St::Stream(port)
    {
        nOnClients = 0;
        lastAcceptedChannel = -1;
        glNewIntRequest = 0;
        registerClients.clear();
        signal(SIGCHLD, reapChild);
    }

    ~Server ()
    {
        registerClients.clear();
    }


    static void reapChild(int unused) 
    {
        while (true) 
        {
            pid_t pid = waitpid(-1, NULL, WNOHANG);
            if (pid <= 0) break; // note the < is now a <=
            glNewIntRequest++;
        }
    }
    /*!
        LoopConnections(): 

        \brief A thread to accept new connections
    */

    static void LoopConnections (Server *serv)
    {
        while (1)
        {
            int newSock = serv->acceptConnection();
            mtxLock.lock();
            lastAcceptedChannel = newSock;
            mtxLock.unlock();
        }
    } 


    /*! \brief Thread to wait for new client requests 
    *   
    *  The basic procedure is as follows:
    *   - Iterates over each registered client
    *   - Creates a child process to receive incoming requests from each client
    *   - Requests are set to the parent process through a pipe
    *   - Once a request is received, the parent handles the request accordingly, and sends the response back to the client
    * 
    *  The basic operation is as follows:
    * 
    * \code{.cpp}  
    *   int r = pipe(client->pipeFd);
    *   pid_t cPid = fork();
    * 
    *   if (cPid == 0) // client requests received in a child process  
    *   {
    *       Packet pkt; 
    *       close (client->pipeFd[0]);
    *       pkt = recvPacket(client);
    *       int r = write (client->pipeFd[1], (void*)&pkt, sizeof(Packet));
    *       close (client->pipeFd[1]);
    *       exit(0);
    *   }
    *
    *   else if (cPid > 0)
    *   {
    *       Packet pkt;
    *       close(client->pipeFd[1]);
    *       int n = read (client->pipeFd[0], (void*)&pkt, sizeof(Packet));
    *       close (client->pipeFd[0]);
    *       //... proceeds to handle the request   
    *   }
    *  \endcode
    */  

    static void LoopRequests (Server *serv)
    {
        std::vector<RegisteredClient>::iterator it;
        size_t nCli = 0;

        while (true)
        {
            mtxLock.lock();

            if (lastAcceptedChannel != -1) // A new client connected
            {
                RegisteredClient newClient;
                newClient.clientSock  = lastAcceptedChannel;
                lastAcceptedChannel   = -1;
                serv->registerClients.push_back(std::move(newClient));
                nCli = serv->registerClients.size();
            }

            mtxLock.unlock();

            for (size_t i = 0; i < nCli; i++)
            {
                serv->loop (i);
            }

            SLEEP_MS(100)
        }
    }

    /*!  void loop (RegisteredClient &client);
        \brief Creates a child process to wait for client requests
    */

    void loop (uint32_t i);
    void sendMsgNoResp (const char *msg, RegisteredClient *client);
    Packet recvPacket (RegisteredClient *client);

private:
    std::vector<RegisteredClient> registerClients;
    uint32_t nOnClients;
};


#endif