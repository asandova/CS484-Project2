//#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <vector>

#include "UDPServer.hpp"

using namespace std;

UDPServer::UDPServer(){
    BufferLength = 512;
    Port = 65535;
    Buffer = vector<char>();
    Buffer.resize(BufferLength, '\0');

    Slength = sizeof(client_addr);

    //Ssocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    Ssocket=socket(AF_INET,SOCK_DGRAM,0);
    if( Ssocket == -1 ){
        perror((char*)Ssocket);
        exit(1);
    }
    bzero( &my_addr, sizeof(my_addr) );
    //memset( (char*) &my_addr,0, sizeof(my_addr) );
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(Port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(Ssocket, (struct sockaddr*)&my_addr, sizeof(my_addr) ) == -1)
    {
        perror("bind");
        exit(1);
    }
}
UDPServer::UDPServer(int port){
    BufferLength = 512;
    Port = port;
    Buffer = vector<char>();
    Buffer.resize(BufferLength, '\0');

    Slength = sizeof(client_addr);

    //Ssocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    Ssocket=socket(AF_INET,SOCK_DGRAM,0);
    if( Ssocket == -1 ){
        perror((char*)Ssocket);
        exit(1);
    }
    bzero( &my_addr, sizeof(my_addr) );
    //memset( (char*) &my_addr,0, sizeof(my_addr) );
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(Port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(Ssocket, (struct sockaddr*)&my_addr, sizeof(my_addr) ) == -1)
    {
        perror("bind");
        exit(1);
    }
}

void UDPServer::echo(){

    int close = 0;
    int counter = 0;
    while(!close){
        if(counter > 3000){
            break;
        }
        Receive();
        string echo = string( (char*)&Buffer[0] );
        if(receiveLength != 0)
            Send( echo );
        counter++;
    }
    if ( shutdown(Ssocket,SHUT_WR) == -1){
        perror("Socket Failed to close: Write\n");
        exit(1);
    }
    if ( shutdown(Ssocket,SHUT_RD) == -1){
        perror("Socket Failed to close: Read\n");
        exit(1);
    }
}

void UDPServer::Receive(){
    cout << "Receiving..." << endl;
    char* buf = &Buffer[0];
    memset(buf, '\0', BufferLength);
    if( ( receiveLength = recvfrom(Ssocket, buf, BufferLength, 0, (struct sockaddr *) &client_addr, &Slength ) ) == -1 ){
        perror("recvfrom()");
        close(Ssocket);
        exit(1);
    }
    cout << "ReceivedLength: " << receiveLength << endl;
    cout << "Received packet from " << inet_ntoa(client_addr.sin_addr)  << ":" << ntohs(client_addr.sin_port) << endl;
    cout << "Data: ";
    for(int i = 0; i < receiveLength && i < BufferLength; i++){
        cout << Buffer[i];
    }
    cout << endl;
}

void UDPServer::Send(string data){
    cout << "Sending..." << endl;
    cout << "Data:" << data << endl;
    if (sendto(Ssocket, data.c_str() , data.size(), 0, (struct sockaddr*) &client_addr, Slength) == -1){
        perror("sendto()");
        close(Ssocket);
        exit(1);
    }

}