# Socket-packet-broker-template

## Description

This repo implements a basic template for C++ socket programming (client/server model) to exchange packets between clients.


## Compiling the code

Run the two makefiles (server and client sides).

## Examples

Run instances of a server (first), followed by the client, as follows


### Server side

First, instantiate a server to listen to a specific port:
```

Server *serv = new Server(PORT);
thread thLoop(Server::LoopRequests, ref(serv));
thConn.join();
```

### Client side

Run a client process to connect to the server:
```
cli = new Client(<some integer id>);
cli->conn(ADDR, PORT);
cli->login("some alias");
```

Then make requests, for example:
```
cli->sendMsg("some message");
cli->insertPacket("Some message", <id of a destination client>);
cli->insertPacket("Other message", <id of a destination client>);
cli->removePacket(0);   // Deletes the packet of index 0
cli->downloadPackets(); // Downloads all packets sent to this client
cli->closeConn();
```

Other possible implemented client requests are shown in Client.h.


