#include "Server.h"


mutex mtxLock;
mutex xxx;
int lastAcceptedChannel;


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


void Server::loop (uint32_t i)
{
    char resp[BUFF_SIZE];
    //int pipeFd[2];

    RegisteredClient *client = &registerClients[i];

    if (client->sockChannel == CHANNEL_FINISHED)  // If the client has already closed the channel, don't wait for its requests
        return; 

    int r = pipe(client->pipeFd);

    pid_t cPid = fork();

    if (cPid == 0) // client requests are handled in a child process  
    {
        Packet pkt; 

        pkt = recvPacket(client);
        close (client->pipeFd[0]);
        int r = write (client->pipeFd[1], (void*)&pkt, sizeof(Packet));
        exit(0);
    }

    else if (cPid > 0)
    {
        int status;
        wait (&status);
        close(client->pipeFd[1]);

        Packet pkt;
        int n = read (client->pipeFd[0], (void*)&pkt, sizeof(Packet));

        printf ("[Request]:\n - Buffer: %s\n - Command: %s\n", (char*)pkt.buffer, cmdStrings[pkt.cmd]);

        if (!pkt.header.valid)
        {
            printf("[!!!] Error: invalid packet ignored...\n");
            sendMsgNoResp("Err. broken packet received", client);
            return;
        }

        if (pkt.cmd != CMD_LOGIN && !client->connected)
        {
            sendMsgNoResp("Err. Client not logged in", client);
            return;
        }

        switch (pkt.cmd) // Evaluates the request, responds accordingly
        {
            case CMD_LOGIN:
            {
                client->connected = true;
                client->id        = nOnClients++; //pkt.header.senderId;

                strcpy (client->alias, (const char*)pkt.buffer);
                pkt.field = client->id;
                sprintf(resp, "Ok, new client logged in %s (id %d)", client->alias, client->id);
                streamSendPacketNoResp(pkt, client->sockChannel);
                break;
            }

            case CMD_LOGOFF:
            {
                sendMsgNoResp ("Ok, client removed.", client);
                
                client->connected = false;
                close(client->sockChannel);
                client->sockChannel = CHANNEL_FINISHED;

                //printf("Ok: closig process\n");
                //exit(0);
                break;
            }

            case CMD_INSERT_PACKET:
            {
                if (pkt.header.receiverId >= nOnClients)
                {
                    sprintf(resp, "Client %2d is not available", pkt.header.receiverId);
                    sendMsgNoResp (resp, client);
                }

                else
                {
                    registerClients[pkt.header.receiverId].insertPacket (pkt);
                    sprintf (resp, "Ok, packet inserted: '%.100s' (queue of cli %u)", (char*)pkt.buffer, pkt.header.receiverId);
                    sendMsgNoResp(resp, client);
                }
                break;
            }

            case CMD_REMOVE_PACKET:
            {
                ssize_t packId = pkt.field;
                client->removePacketById (packId);
                sprintf (resp, "Ok, packet %3lu removed", packId);
                sendMsgNoResp(resp, client);
                break;
            }

            case CMD_GET_NUMBER_OF_PACKETS:
            {
                pkt.field = client->msgQueue.packets.size();
                streamSendPacketNoResp(pkt, client->sockChannel);
                break;
            }

            case CMD_DOWNLOAD_PACKETS:
            {
                if (client->msgQueue.packets.size() != 0)
                {
                    sendMsgNoResp("Sending packets", client);

                    for (Packet pkt: client->msgQueue.packets)
                        streamSendPacketNoResp(pkt, client->sockChannel);
                }

                else
                    sendMsgNoResp("There are no packets to download", client);
                break;
            }

            case CMD_CLEAR_PACKETS:
            {
                client->clearPackets();
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
            default: // TODO
                break;
        }   
    }

    else
    {
        printf ("Fork error...\n");
        exit(0);
    }
}


Packet Server::recvPacket (RegisteredClient *client)
{
    return streamRecvPacket (client->sockChannel);
}


void Server::sendMsgNoResp (const char *msg, RegisteredClient *client)
{
    Packet pSend;
    strcpy ((char*)pSend.buffer, msg);
    streamSendPacketNoResp (pSend, client->sockChannel);
}
