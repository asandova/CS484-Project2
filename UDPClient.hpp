#ifndef UDPCLIENT_HPP
#define UDPCLIENT_HPP

#include <arpa/inet.h>
#include <sys/socket.h>
#include <vector>
#include <string>

using namespace std;

class UDPClient{

    private:
        string ServerAddress;
        int BufferLength;
        int ReceiveLength;
        int Port;
        int Ssocket;
        vector<char>Buffer;
        socklen_t Slength;
    
        struct sockaddr_in my_addr;
        struct sockaddr_in server_addr;

    public:
        UDPClient(string ip, int port);
        void echo();
    private:
        void Send(string data);
        void Receive();


};
#endif //UDPCLIENT_HPP