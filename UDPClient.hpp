/*
*   File: UDPClient.hpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPClient class Declaration
*   Class: CS484
*/
#ifndef UDPCLIENT_HPP
#define UDPCLIENT_HPP

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

class UDPClient{

    private:
        bool debugMode;
        string ServerAddress;
        int BufferLength;
        int ReceiveLength;
        time_t lastSent;
        int tries;
        int Port;
        int Ssocket;
        string Buffer;
        struct UDPData receivedData;
        socklen_t Slength;
        struct timeval waitTime;

        struct sockaddr_in my_addr;
        struct sockaddr_in server_addr;

    public:
        static bool DebugMode;
        static bool verboseMode;
        UDPClient(string ip, int port);
        UDPClient(string ip, int port, int bufferLen);
        void echo();
        void save();
        void terminateClient();
    private:
        void Send(string data);
        void Receive();
        void closeSocket();

};
#endif //UDPCLIENT_HPP