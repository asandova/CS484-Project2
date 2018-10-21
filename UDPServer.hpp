#ifndef UDPSERVER_HPP
#define UDPSERVER_HPP

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//#include <netinet/in.h>
#include <unistd.h>
#include <vector>

using namespace std;

class UDPServer{

    private:
        int Port;
        int bufferLength;
        vector<char>buffer;
        int Ssocket;
        int receiveLength;
        socklen_t Slength;
        struct sockaddr_in my_addr;
        struct sockaddr_in client_addr;


    public:
        UDPServer();
        //UDPServer(/**/);
        void waiting();

    private:
        void Send(/*data struct*/);
        void Receive();

};
#endif //UDPSERVER_HPP