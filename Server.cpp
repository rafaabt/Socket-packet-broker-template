#include "Server.h"


mutex mtxLock;

const char *cmdStrings[20] = 	
{
	"CMD_MSG",
	"CMD_LOGIN",
	"CMD_INSERT_PACKET",
	"CMD_REMOVE_PACKET",
	"CMD_CLEAR_PACKETS",
	"CMD_LOGOFF",
	"CMD_LIST_CLI",
	"CMD_DOWNLOAD_PACKETS",
	"CMD_GET_NUMBER_OF_PACKETS"
};


void Server::loop (RegisteredClient &client)
{
	char resp[500];
	int fd[2];

	if (client.sockChannel == FINISHED)
		return;	

	pipe(fd);
	pid_t cPid = fork();

	if (cPid == 0) // client requests are handled in a child process  
	{			
		Packet pkt; 
	
		pkt = recvPacket(client);
		
		close (fd[0]);
		close (1);
		dup(fd[1]);

		write (fd[1], (void*)&pkt, sizeof(Packet));
		exit(1);
	}

	else if (cPid > 0)
	{
		waitpid(cPid, NULL, 0);
		close(0);
		close(fd[1]);
		dup(fd[0]);

		Packet pkt;

		int n = read (fd[0], (void*)&pkt, sizeof(Packet));

		printf ("[Request]:\n - Buffer: %s\n - Command: %s\n", (char*)pkt.buffer, cmdStrings[pkt.cmd]);

		if (!pkt.header.valid)
		{
			printf("[!!!] Error: invalid packet ignored...\n");
			//sendMsgNoResp("Err. broken packet received", client);
			return; //continue;
		}

		if (pkt.cmd != CMD_LOGIN && !client.connected)
		{
			sendMsgNoResp("Err. Client not logged in", client);
			return; //continue;
		}

		switch (pkt.cmd) // Evaluates the request, responds accordingly
		{
			case CMD_LOGIN:
			{
				client.connected = true;
				client.id        = nOnClients++; //pkt.header.senderId;
				mapClient[client.id] = &client;

				strcpy (client.alias, (const char*)pkt.buffer);
				pkt.field = client.id;
				sprintf(resp, "Ok, new client logged in %s (id %d)", client.alias, client.id);
				streamSendPacketNoResp(pkt, client.sockChannel);
				break;
			}

			case CMD_LOGOFF:
			{
				sendMsgNoResp ("Ok, client removed.", client);
				unregisterClient (client); 
				
				//printf("Ok: closig process\n");
				//exit(0);
				break;
			}

			case CMD_INSERT_PACKET:
			{
				if (mapClient.find(pkt.header.receiverId) == mapClient.end())
				{
					sprintf(resp, "Client %2d is not available", pkt.header.receiverId);
					sendMsgNoResp (resp, client);
				}
				else
				{
					mapClient[pkt.header.receiverId]->insertPacket (pkt);
					sprintf (resp, "Ok, packet inserted: '%.100s' (queue cli id %u)", (char*)pkt.buffer, pkt.header.receiverId);
					sendMsgNoResp(resp, client);
				}
				break;
			}

			case CMD_REMOVE_PACKET:
			{
				ssize_t packId = pkt.field;
				client.removePacketById (packId);
				sprintf (resp, "Ok, packet %3lu removed", packId);
				sendMsgNoResp(resp, client);
				break;
			}

			case CMD_GET_NUMBER_OF_PACKETS:
			{
				pkt.field = client.msgQueue.packets.size();
				streamSendPacketNoResp(pkt, client.sockChannel);
				break;
			}

			case CMD_DOWNLOAD_PACKETS:
			{
				if (client.msgQueue.packets.size() != 0)
				{
					for (Packet sendPacket: client.msgQueue.packets)
						streamSendPacketNoResp(sendPacket, client.sockChannel);
				}

				else
					sendMsgNoResp("There are no packets to download", client);
				
				break;
			}

			case CMD_CLEAR_PACKETS:
			{
				client.clearPackets();
				sprintf (resp, "Ok, all packets deleted");
				sendMsgNoResp(resp, client);
				break;
			}

			case CMD_MSG:
			{
				sprintf (resp, "Ok, received msg '%.100s'", (char*)pkt.buffer);
				sendMsgNoResp(resp, client);
				break;
			}
			default:
			{	// TODO
				break;
			}
		}
	}
}


void Server::unregisterClient (RegisteredClient &client) // does not work
{
	client.connected = false;
	close(client.sockChannel);
	client.sockChannel = FINISHED;
}


Packet Server::recvPacket (RegisteredClient& client)
{
	return streamRecvPacket (client.sockChannel);
}


void Server::sendMsgNoResp (const char *msg, RegisteredClient& client)
{	
	Packet pSend;
	strcpy ((char*)pSend.buffer, msg);
	streamSendPacketNoResp (pSend, client.sockChannel);
}
