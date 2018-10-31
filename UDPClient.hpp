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
        string ServerAddress;
        unsigned int BufferLength;
        int ReceiveLength;
        time_t lastSent;
        int tries;
        int Port;
        static int Ssocket;
        string Buffer;
        UDPData receivedData;
        socklen_t Slength;
        struct timeval waitTime;

        struct sockaddr_in my_addr;
        struct sockaddr_in server_addr;

    public:
        static bool DebugMode;
        static bool verboseMode;
        UDPClient(string ip, int port, bool useHostName);
        UDPClient(string ip, int port, unsigned int bufferLen, bool useHostName);
        //void echo();
        void save(string filename);
        int run();
        static void terminateClient(int signum);
    private:
        void Send(string data);
        void Receive();
        static void closeSocket();
        string getHostIp(string name);

};
#endif //UDPCLIENT_HPP