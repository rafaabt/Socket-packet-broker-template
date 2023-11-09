# Socket-packet-broker-template

## Description

This repository implements a basic template for C++ socket programming (client/server model) to exchange packets between clients.
Packets are generic and simple structures composed of a header, a command to the server, a general-purpose integer field and some bytes of generic type data.

## Compiling the code
Run the two makefiles (server and client sides).


## Runnig the applications
Run instances of a server (first), followed by the client running in another terminal.


## Code examples

### Server side

The server listens to new incoming connections on a dedicated thread. Once new connections are accepted, the server
creates a per-client child process to serve client requests. First, instantiate a server to listen to a specific port:
```
#define PORT 8080

Server *serv = new Server(PORT);
thread thLoop(Server::LoopRequests, ref(serv)); // the waits for new connections in a dedicated thread
thConn.join();
```

### Client side

Run a client process to connect to the server:
```
#define ADDR "127.0.0.1"
#define PORT 8080

Client *cli = new Client();
cli->conn(ADDR, PORT);
cli->login("Some alias");
```

Then make requests, for example:
```
cli->sendMsg("Some message to the server"); 
cli->insertPacket("Some message to client of id 42", 42);
cli->insertPacket("Other message to client of id 55", 55);
cli->removePacket(0);   // Deletes the packet of index 0
cli->downloadPackets(); // Downloads all packets sent to this client
cli->closeConn();
```

Other possible implemented client requests are shown in Client.h.


