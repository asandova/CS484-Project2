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

typedef struct OpenConnections{
    int PacketLength;
    int position;
    struct sockaddr_in address;
    socklen_t Slen;
    UDPData toSend;
    clock_t lastSent;
    int tries;
} Connections;

class UDPServer{

    private:
        int Port;
        static int Ssocket;
        int receiveLength;
        string dataToServe;
        int BufferLength;
        string Buffer;
        struct timeval TimeInterval;
        socklen_t Slength;
        vector<Connections> Clients;

        struct sockaddr_in my_addr;
        struct sockaddr_in client_addr;

    public:
        static bool DebugMode;
        static bool verboseMode;
        UDPServer(string filename);
        UDPServer(string filename, int port);
        static void terminateServer(int signum);
        void run();
        //void echo();

    private:
        void Send(string data, struct sockaddr_in client, socklen_t clen);
        void Receive();
        static void closeSocket();
        void readFile(string filename);
        void fileToUDP(UDPData &cblocks,int len);
};
#endif //UDPSERVER_HPP