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


using namespace std;


extern mutex mtxLock;


//! Server class  
/*!   
* This class models a simple server for socket programming. 
**/


class Server: public Stream
{
public:
    Server (ssize_t port):Stream(port)
    {
        nOnClients = 0;
        registerClients.clear();
    }

    ~Server ()
    {
        registerClients.clear();
    }


    /*!
        LoopConnections(): 

        \brief A thread to accept new connections
    */

    static void LoopConnections (Server *serv)
    {
        socklen_t addrlen = sizeof(serv->address);

        while (1)
        {
            int newSock;

            if ((newSock = accept(serv->sockServConn, (struct sockaddr*)&serv->address, &addrlen)) < 0) 
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            RegisteredClient newClient;

            newClient.sockChannel = newSock;

            mtxLock.lock();
            serv->registerClients.push_back(newClient);
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
    *  \code{.cpp}
    *   
    *   int fd[2];
    *
    *   if (client.sockChannel == CHANNEL_FINISHED) // If the client has already closed the channel, don't wait for its requests
    *        return; 
    *
    *    pipe(fd);
    *    pid_t cPid = fork();
    *
    *    if (cPid == 0) // client requests are handled in a child process  
    *    {
    *        Packet pkt; 
    *        pkt = recvPacket(client);
    *        close (fd[0]);
    *        write (fd[1], (void*)&pkt, sizeof(Packet)); // Sends requests to the parent process
    *        exit(0);
    *    }
    *
    *    else if (cPid > 0) // Parent process
    *    {
    *        int status;
    *        wait (&status);
    *        close(fd[1]);
    *        Packet pkt;
    *        int n = read (fd[0], (void*)&pkt, sizeof(Packet)); // Gets the packet from the child process
    *        //...proceeds to handle the packet
    *        // ...  
    *   }
    *  \endcode
    */  

    static void LoopRequests (Server *serv)
    {
        while (true)
        {
            vector<RegisteredClient>::iterator it;
            size_t nCli;

            mtxLock.lock();
            nCli = serv->registerClients.size();
            mtxLock.unlock();

            for (int i = 0; i < nCli; i++)
            {
                mtxLock.lock();
                serv->loop (serv->registerClients[i]);
                mtxLock.unlock();
            }

            sleep(1);
        }
    }

    /*!  void loop (RegisteredClient &client);
        \brief Creates a child process to wait for client requests
    */

    void loop (RegisteredClient &client);
    void sendMsgNoResp (const char *msg, RegisteredClient& client);
    Packet recvPacket (RegisteredClient& client);
    void unregisterClient (RegisteredClient &client);

private:
    vector<RegisteredClient> registerClients;
    map<uint32_t, RegisteredClient*> mapClient;
    uint32_t nOnClients;
};


#endif