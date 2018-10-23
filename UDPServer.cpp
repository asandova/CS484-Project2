/*
*   File: UDPServer.cpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPServer class Definition
*   Class: CS484
*/

//Socket headers
#include <sys/types.h>
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

bool UDPServer::DebugMode = false;
bool UDPServer::verboseMode = false;

UDPServer::UDPServer(){
    /*
        default constructor for UDPServer object
    */
    BufferLength = 512;
    Port = 65535;
    Buffer = vector<char>();
    Buffer.resize(BufferLength, '\0');

    Slength = sizeof(client_addr);

    Ssocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if( Ssocket == -1 ){
        perror((char*)Ssocket);
        exit(1);
    }
    //bzero( &my_addr, sizeof(my_addr) );
    memset( (char*) &my_addr,0, sizeof(my_addr) );
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

    Ssocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if( Ssocket == -1 ){
        perror((char*)Ssocket);
        exit(1);
    }
    //bzero( &my_addr, sizeof(my_addr) );
    memset( (char*) &my_addr,0, sizeof(my_addr) );
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
    /*
        Does a simple echo procedure
    */
    int close = 0;
    int counter = 0;
    while(counter < 1){
        Receive();
        string echo = string( (char*)&Buffer[0] );
        Send( echo );
        counter++;
    }
    closeSocket();
}

void UDPServer::Receive(){
    /*
        Receives incoming UDP data from socket
    */
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
    /*
        Send data to desired address
    */
    cout << "Sending..." << endl;
    cout << "Data:" << data << endl;
    int sentlength;
    if ( (sentlength =  sendto(Ssocket, data.c_str() , data.size(), 0, (struct sockaddr*) &client_addr, Slength)) == -1){
        perror("sendto()");
        close(Ssocket);
        exit(1);
    }
    cout << "sent Length:" << sentlength << endl;

}
void UDPServer::closeSocket(){
    /*
        Properly closes the socket
    */
    if (close(Ssocket) == -1){
        perror("Socket Closing Error: ");
        exit(1);
    }
}