/*
*   File: UDPServer.hpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPServer class Declaration
*   Class: CS484
*/
#ifndef UDPSERVER_HPP
#define UDPSERVER_HPP

#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include "UDPDataBlock.hpp"

using namespace std;

struct OpenConnections{
    int placement;
    struct sockaddr_in address;
    struct UDPData toSend;
};

class UDPServer{

    private:
        bool debugMode;
        int BufferLength;
        int Port;
        int Ssocket;
        int receiveLength;
        vector<char> Buffer;
        struct timeval TimeInterval;
        socklen_t Slength;
        vector<struct OpenConnections> Active;
        UDPData dataToSend;

        struct sockaddr_in my_addr;
        struct sockaddr_in client_addr;

    public:
        static bool DebugMode;
        static bool verboseMode;
        UDPServer(string filename);
        UDPServer(string filename, int port);
        void run();
        void handShake();
        void echo();

    private:
        void Send(string data);
        void Receive();
        void closeSocket();

};
#endif //UDPSERVER_HPP