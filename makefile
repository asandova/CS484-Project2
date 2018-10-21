all: client server

client: UDPClient.cpp UDPClient.hpp
	g++ -c UDPClient.cpp

server: UDPServer.hpp UDPServer.cpp
	g++ -c UDPServer.cpp