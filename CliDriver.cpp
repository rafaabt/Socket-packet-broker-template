// Client side C/C++ program to demonstrate Socket programming

#include <iostream>
#include "Client.h"

#define ADDR       "127.0.0.1"
#define PORT       8080
#define N_CLIENTS  2
#define N_PACKETS  10


using namespace std;


int main(int argc, char const* argv[])
{		
	Client *cli[N_CLIENTS];
	char alias[20];

	for (int i = 0; i < N_CLIENTS; i++)
	{
		cli[i] = new Client(i);
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
			sprintf (str, "Message %d ", j);
			cli[i]->insertPacket(str, 0); //sendMsg(str);
		}
	}

	printf("Downloaded packets: \n");
	for (int i = 0; i < N_CLIENTS; i++)
	{
		cli[i]->downloadPackets();
	}


	for (int i = 0; i < N_CLIENTS; i++)
	{
		cli[i]->removePacket(4);
		cli[i]->removePacket(5); 
		//cli[i]->clearPackets();
	}

	printf("Downloaded packets after clear: \n");
	for (int i = 0; i < N_CLIENTS; i++)
	{
		cli[i]->downloadPackets();
	}

	for (int i = 0; i < N_CLIENTS; i++)
	{
		cli[i]->closeConn();
		delete cli[i];
	}

    return 0;
}