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
#include <sys/time.h>

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

UDPServer::UDPServer(string filename){
    BufferLength = 512;
    Port = 65535;
    Buffer = vector<char>();
    Buffer.resize(BufferLength, '\0');
    Active = vector<struct OpenConnections>();
    TimeInterval.tv_usec = 500;

    dataToSend.parseFile(filename);

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
UDPServer::UDPServer(string filename, int port){
    BufferLength = 512;
    Port = port;
    Buffer = vector<char>();
    Buffer.resize(BufferLength, '\0');
    Active = vector<struct OpenConnections>();
    TimeInterval.tv_usec = 500;

    dataToSend.parseFile(filename);

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
    if(debugMode||verboseMode){
        cout << "Socket " << Ssocket << " was sucsessfully binded" << endl;
    }
    
}
void UDPServer::run(){
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(Ssocket, &rfds);
    int running = 1;
    while(running){
        int selRet = select(Ssocket+1, &rfds, NULL,NULL, &TimeInterval);
        if(selRet == -1){
            //error
            perror("bind");
            closeSocket();
            exit(1);
        }
        else if(selRet == 0){
            //timeout
        }else{
            Receive();
            vector<struct OpenConnections>::iterator itr;
            for(itr = Active.begin(); itr != Active.end(); ++itr){
                if( client_addr != *itr ){
                    //do handshake
                        //send number total number of packets
                    break;
                }else if(client_addr == *itr){
                    //check ack
                        //this is the number of the last received block
                    //send new block or resend

                    break;
                }
            }
        }
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
    if(debugMode || verboseMode) {cout << "Receiving..." << endl;}
    char* buf = &Buffer[0];
    memset(buf, '\0', BufferLength);
    if( ( receiveLength = recvfrom(Ssocket, buf, BufferLength, 0, (struct sockaddr *) &client_addr, &Slength ) ) == -1 ){
        perror("recvfrom()");
        close(Ssocket);
        exit(1);
    }
    if(debugMode || verboseMode){
        cout << "ReceivedLength: " << receiveLength << endl;
        cout << "Received packet from " << inet_ntoa(client_addr.sin_addr)  << ":" << ntohs(client_addr.sin_port) << endl;
        cout << "Data: ";
        for(int i = 0; i < receiveLength && i < BufferLength; i++){
            cout << Buffer[i];
        }
        cout << endl;
    }
}

void UDPServer::Send(string data){
    /*
        Send data to desired address
    */
   if(debugMode || verboseMode){
        cout << "Sending..." << endl;
        cout << "Data:" << data << endl;
    }
    int sentlength;
    if ( (sentlength =  sendto(Ssocket, data.c_str() , data.size(), 0, (struct sockaddr*) &client_addr, Slength)) == -1){
        perror("sendto()");
        close(Ssocket);
        exit(1);
    }
    if(debugMode || verboseMode){cout << "sent Length:" << sentlength << endl;}

}
void UDPServer::closeSocket(){
    /*
        Properly closes the socket
    */
    if(debugMode || verboseMode){ cout << "closing socket " << Ssocket << endl;}
    if (close(Ssocket) == -1){
        perror("Socket Closing Error: ");
        exit(1);
    }
    if(debugMode || verboseMode){ cout << "Socket " << Ssocket  << " successfuly closed"<< endl;}
}