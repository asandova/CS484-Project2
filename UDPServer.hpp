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
    //this struct contains is what maintains active connect for the server
    //It hold where the client is in data transfer and what size of packets the client wants
    //As well as storing the clients address
    int PacketLength;
    int position;
    struct sockaddr_in address;
    socklen_t Slen;
    UDPData toSend;
    clock_t lastSent;
    int tries;
} Connections;

Connections makeConnecton(int PackLen, int pos, struct sockaddr_in addr, socklen_t Slen, UDPData toSend, clock_t lastSent, int tries);

class UDPServer{

    private:
        int Port;//stores the server port number
        static int Ssocket;//stores the server binded socket
        int receiveLength; //stores the length of each reveived packet
        string dataToServe; //stores the read in file data. This be divided at need
        int BufferLength;//is the receiveing buffer lenght
        string Buffer;//this is the receiving buffer
        struct timeval TimeInterval; //the time interal for waiting for packets each interation
        socklen_t Slength;//length of the clients address
        vector<Connections> Clients;//list of clients

        struct sockaddr_in my_addr;//stores server address
        struct sockaddr_in client_addr;//stores client addres. this is a holding varaible

    public:
        static bool DebugMode;
        static bool verboseMode;
        UDPServer(string filename);//constructor
        UDPServer(string filename, int port);//constructro
        static void terminateServer(int signum);//used to catch the control^C interupt
        void run();//runs the server
        //void echo();

    private:
        void Send(string data, struct sockaddr_in client, socklen_t clen);
        void Receive();
        static void closeSocket();
        void readFile(string filename);
        void fileToUDP(UDPData &cblocks,int len);
};
#endif //UDPSERVER_HPP