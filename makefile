all: serverDriver clientDriver

clientDriver: cdriver client
	g++ clientdriver.o UDPClient.o -o client

cdriver: clientdriver.cpp
	g++ -c clientdriver.cpp

client: UDPClient.cpp UDPClient.hpp
	g++ -c UDPClient.cpp


serverDriver: sdriver server
	g++ serverdriver.o UDPServer.o -o server

sdriver: serverdriver.cpp
	g++ -c serverdriver.cpp

server: UDPServer.hpp UDPServer.cpp
	g++ -c UDPServer.cpp

clean:
	rm -rf *.o
	rm server
	rm client