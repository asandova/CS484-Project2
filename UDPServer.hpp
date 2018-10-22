#ifndef UDPSERVER_HPP
#define UDPSERVER_HPP

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>

using namespace std;

class UDPServer{

    private:
        
        int BufferLength;
        int Port;
        int Ssocket;
        int receiveLength;
        vector<char>Buffer;
        socklen_t Slength;

        struct sockaddr_in my_addr;
        struct sockaddr_in client_addr;


    public:
        UDPServer();
        UDPServer(int port);
        void echo();

    private:
        void Send(string data);
        void Receive();
        void closeSocket();

};
#endif //UDPSERVER_HPP