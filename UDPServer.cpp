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
    bufferLength = 512;
    Port = 65535;
    buffer = vector<char>();
    buffer.reserve(bufferLength);

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

void UDPServer::waiting(){

    int close = 0;
    while(!close){
        Receive();
        cout << "Received packet from " << inet_ntoa(client_addr.sin_addr)  << ":" << ntohs(client_addr.sin_port) << endl;
        //printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        cout << "Date: ";
        vector<char>::iterator itr;
        for(itr = buffer.begin(); itr != buffer.end(); ++itr){
            cout << *itr;
        }
        cout << endl;
        //cout << "Date: " << buffer << endl;
        //printf("Data: %s\n" , buffer);
        Send();
    }
}

void UDPServer::Receive(){

    char* temp = &buffer[0];
    if( ( receiveLength = recvfrom(Ssocket, temp, bufferLength, 0, (struct sockaddr *) &client_addr, &Slength ) ) == -1 ){
        perror("recvfrom()");
        exit(1);
    }
}

void UDPServer::Send(){

    char* temp = &buffer[0];
    if (sendto(Ssocket, temp, receiveLength, 0, (struct sockaddr*) &client_addr, Slength) == -1){
        perror("sendto()");
        exit(0);
    }
}