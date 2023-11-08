# Socket-packet-broker-template

## Description

This repo implements a basic template for C++ socket programming (client/server model) to exchange packets between clients.


## Compiling the code
Run the two makefiles (server and client sides).


## Runnig the applications
Run instances of a server (first), followed by the client running in another terminal, as follows:
```
./server
./client
```

## Code examples

### Server side

First, instantiate a server to listen to a specific port:
```
Server *serv = new Server(PORT);
thread thLoop(Server::LoopRequests, ref(serv)); // waits for new connections on a dedicated thread
thConn.join();
```

### Client side

Run a client process to connect to the server:
```
Client *cli = new Client(<some integer id>);
cli->conn(ADDR, PORT);
cli->login("some alias");
```

Then make requests, for example:
```
cli->sendMsg("Some message"); 
cli->insertPacket("Some message to client of id 42", 42);
cli->insertPacket("Other message to client of id 55", 55);
cli->removePacket(0);   // Deletes the packet of index 0
cli->downloadPackets(); // Downloads all packets sent to this client
cli->closeConn();
```

Other possible implemented client requests are shown in Client.h.


