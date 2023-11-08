#include "Server.h"

mutex mtxLock;

void Server::unregisterClient (RegisteredClient &client) // does not work
{
	//mtxLock.lock();
	close(client.sockChannel);
	client.connected = false;
	nOnClients--; // TODO:	 problema: a thread de conexões escreve nesta variável

	for (vector<RegisteredClient>::iterator it = registerClients.begin(); it != registerClients.end(); it++)
	{
		RegisteredClient r = *it;

		if (r.id == client.id)
		{
			registerClients.erase(it);
			break;
		}
	}
	//mtxLock.unlock();
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


void Server::loop (RegisteredClient &client)
{
	pid_t cPid = fork();

	if (cPid == 0) // client requests are handled in a child process  
	{			 
		char str[BUFF_SIZE];

		while (1)
		{
			//printf("Server awaiting msg from client %d\n", client.id);

			Packet pkt = recvPacket(client);

			if (!pkt.header.valid)
			{
				printf("[!!!] Error: invalid packet ignored...\n");
				continue;
			}
		
			if (pkt.cmd != CMD_LOGIN && !client.connected)
			{
				sprintf(str, "Err. Client not logged\n");
				sendMsgNoResp(str, client);
				continue;
			}

			switch (pkt.cmd) // Evaluates the request, responds accordingly
			{
				case CMD_LOGIN:
				{
					client.connected = true;
					strcpy (client.alias, (const char*)pkt.buffer);
					sprintf(str, "Ok, new client logged in %s\n", client.alias);
					sendMsgNoResp(str, client);
					break;
				}
				case CMD_DOWNLOAD_PACKETS:
				{
					if (client.msgQueue.nPackets != 0)
					{
						bool inboxEmpty = true;

						for (Packet sendPacket: client.msgQueue.packets)
						{
							if (sendPacket.header.receiverId == client.id)
							{
								inboxEmpty = false;
								streamSendPacketNoResp(sendPacket, client.sockChannel);
							}
						}

						if (inboxEmpty)
						{
							sprintf (str, "Empty inbox for client %d", client.id);
							sendMsgNoResp(str, client);
						}
					}

					else
						sendMsgNoResp("There are no packets to download", client);
					
					break;
				}

				case CMD_UNREG_CLI:
				{
					sendMsgNoResp ("Ok, client removed...\n", client);
					//unregisterClient (client); // TODO: Fix
					
					printf("Ok: closig process\n");
					exit(0);
					break;
				}

				case CMD_GET_NUMBER_OF_PACKETS:
				{
					uint32_t count = 0;

					for (Packet sendPacket: client.msgQueue.packets)
					{
						if (sendPacket.header.receiverId == client.id)
							count++;
					}

					pkt.field = count;
					streamSendPacketNoResp(pkt, client.sockChannel);
					break;
				}

				case CMD_INSERT_PACKET:
				{
					client.insertPacket (pkt);
					sprintf (str, "Ok, packet inserted: '%.100s'", (char*)pkt.buffer);
					sendMsgNoResp(str, client);
					break;
				}

				case CMD_REMOVE_PACKET:
				{
					ssize_t packId = pkt.field;
					client.removePacketById (packId);
					sprintf (str, "Ok, packet %3lu removed", packId);
					sendMsgNoResp(str, client);
					break;
				}

				case CMD_CLEAR_PACKETS:
				{
					client.clearPackets();
					sprintf (str, "Ok, all packets deleted");
					sendMsgNoResp(str, client);
					break;
				}

				case CMD_MSG:
				{
					sprintf (str, "Ok, received msg '%.100s'", (char*)pkt.buffer);
					sendMsgNoResp(str, client);
					break;
				}
				default:
				{	// TODO
					break;
				}
			}
		}
	}
}