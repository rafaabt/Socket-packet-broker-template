CC=g++
OPT=-O0


# $@ means the target
# $< means just the first prerequisite
# $^ means all pre-requisites

server: ServDriver.cpp Server.o Stream.a
	${CC} $^ -lpthread -o $@

Server.o: Server.cpp Stream.a Server.h RegisteredClients.h Stream.h Packet.h utils.h
	${CC} $< ${OPT} -c

client: CliDriver.cpp Client.o Stream.a
	${CC} $^ -o $@

Client.o: Client.cpp Stream.a Client.h Stream.h Packet.h utils.h
	${CC} $< ${OPT} -c


Stream.a: Stream.o
	ar rcs $@ $<

Stream.o: Stream.cpp Stream.h
	${CC} $< ${OPT} -c

clean: 
	rm *.o
