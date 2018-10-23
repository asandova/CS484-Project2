/*
*   File: UDPServer.hpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPServer class Definition
*   Class: CS484
*/
#ifndef UDPSERVER_HPP
#define UDPSERVER_HPP

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>

using namespace std;



class UDPServer{

    private:
        bool debugMode;
        int BufferLength;
        int Port;
        int Ssocket;
        int receiveLength;
        vector<char>Buffer;
        socklen_t Slength;

        struct sockaddr_in my_addr;
        struct sockaddr_in client_addr;


    public:
        static bool DebugMode;
        static bool verboseMode;
        UDPServer();
        UDPServer(int port);
        void echo();

    private:
        void Send(string data);
        void Receive();
        void closeSocket();

};
#endif //UDPSERVER_HPP