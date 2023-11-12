CC=g++
OPT=-O0


# $@ means the target
# $< means just the first prerequisite
# $^ means all pre-requisites

server: ServDriver.cpp Server.o
	${CC} $^ -lpthread -o $@

Server.o: Server.cpp Server.h RegisteredClients.h Stream.h Packet.h utils.h
	${CC} $< ${OPT} -c

client: CliDriver.cpp Client.o
	${CC} $^ -o $@

Client.o: Client.cpp Client.h Stream.h Packet.h utils.h
	${CC} $< ${OPT} -c

clean: 
	rm *.o
