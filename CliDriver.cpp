// Client side C/C++ program to demonstrate Socket programming

#include <iostream>
#include <csignal>
#include "Client.h"

#define ADDR       "127.0.0.1"
#define PORT       8080
#define N_PACKETS  2


using namespace std;

Client *cli;

static void closeClients(int unused) // Handles interrupts (e.g., from keyboard)
{
    cli->closeConn();
    delete cli;
    exit(0);
}


int main(int argc, char const* argv[])
{		
    char alias[20];

    signal(SIGINT, closeClients);

#if 0 
    Client c2({
        .mConnect = [](auto *p)
        { 
            struct UserData *user = (UserData*)p;
            cout << "Connecting\n";
            cout << "user: " << user->id << "\ndata: " << user->data << endl;
        },
        .mSend = []()
        { 
            cout << "Sending\n"; 
        },
        .mRecv = []()
        { 
            cout << "Receiving\n"; 
        }
    });

    UserData u = 
    {
        .id = 42,
        .data = 4242
    };

    c2.mConnect(&u);

    c2.mSend();
    c2.mRecv();
    exit(0);
#endif

    cli = new Client();
    cli->conn(ADDR, PORT);
    
    sprintf (alias, "@cli");
    cli->login(alias);

    for (int i = 0; i < N_PACKETS; i++)
    {
        char str[100];
        sprintf (str, "Message %d from client %i", i, cli->getId());
        //cli->insertPacket(str, 0); //cli->getId()); 
        cli->sendMsg(str);
    }
    
#if 0
    printf("Downloaded packets: \n");

    cli->downloadPackets();
    
#if 0
    cli->removePacket(4);
    cli->removePacket(5); 
    //cli->clearPackets();
    

    printf("Downloaded packets after clear: \n");

    cli->downloadPackets();
#endif
#endif

    cli->closeConn();

    delete cli;
    

    return 0;
}