/*
*   File: UDPClient.hpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPClient class Declaration
*   Class: CS484
*/
#ifndef UDPCLIENT_HPP
#define UDPCLIENT_HPP

#include <arpa/inet.h>
#include <sys/socket.h>
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
        int Port;
        int Ssocket;
        vector<char>Buffer;
        vector<struct UDPData> receivedData;
        socklen_t Slength;
    
        struct sockaddr_in my_addr;
        struct sockaddr_in server_addr;

    public:
        static bool DebugMode;
        static bool verboseMode;
        UDPClient(string ip, int port);
        void echo();
        void save();
    private:
        void Send(string data);
        void Receive();
        void closeSocket();

};
#endif //UDPCLIENT_HPP