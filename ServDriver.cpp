// Server side C/C++ program to demonstrate Socket programming

#include <iostream>
#include <thread> 
#include "Server.h"

#define PORT 8080


int main(int argc, char const* argv[])
{
    Server *serv = new Server(PORT);
    std::thread thConn(Server::LoopConnections, std::ref(serv));
    std::thread thLoop(Server::LoopRequests,    std::ref(serv));

    thConn.join();
    thLoop.join();

    delete serv;
    return 0;
}
