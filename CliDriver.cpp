// Client side C/C++ program to demonstrate Socket programming

#include <iostream>
#include <csignal>
#include "Client.h"

#define ADDR       "127.0.0.1"
#define PORT       8080
#define N_CLIENTS  1
#define N_PACKETS  2


using namespace std;

Client *cli[N_CLIENTS];

static void closeClients(int unused) // Handles interrupts (e.g., from keyboard)
{
    for (int i = 0; i < N_CLIENTS; i++)
    {
        cli[i]->closeConn();
        delete cli[i];
    }
    exit(0);
}


int main(int argc, char const* argv[])
{		
    char alias[20];

    signal(SIGINT, closeClients);

    for (int i = 0; i < N_CLIENTS; i++)
    {
        cli[i] = new Client();
        cli[i]->conn(ADDR, PORT);
    }

    for (int i = 0; i < N_CLIENTS; i++)
    {
        sprintf (alias, "@cli_%2d", i);
        cli[i]->login(alias);
    }

    for (int i = 0; i < N_CLIENTS; i++)
    {
        for (int j = 0; j < N_PACKETS; j++)
        {
            char str[100];
            sprintf (str, "Message %d from client %i", j, cli[i]->getId());
            //cli[i]->insertPacket(str, 0); //cli[0]->getId()); 
            cli[i]->sendMsg(str);
        }
    }

#if 0
    printf("Downloaded packets: \n");

    for (int i = 0; i < N_CLIENTS; i++)
    {
        cli[i]->downloadPackets();
    }
#if 0
    for (int i = 0; i < N_CLIENTS; i++)
    {
        cli[i]->removePacket(4);
        cli[i]->removePacket(5); 
        //cli[i]->clearPackets();
    }

    printf("Downloaded packets after clear: \n");
    for (int i = 0; i < N_CLIENTS; i++)
        cli[i]->downloadPackets();
#endif
#endif

    for (int i = 0; i < N_CLIENTS; i++)
    {
        cli[i]->closeConn();
        delete cli[i];
    }

    return 0;
}