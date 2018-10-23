/*
*   File: UDPClient.cpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPClient class Definition
*   Class: CS484
*/
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>

#include "UDPClient.hpp"

using namespace std;

bool UDPClient::DebugMode = false;
bool UDPClient::verboseMode = false;

UDPClient::UDPClient(string ip, int port){
    BufferLength = 512;
    Port = port;
    Buffer = vector<char>();
    Buffer.resize(BufferLength, '\0');

    Slength = sizeof(server_addr);

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr );

    if( (Ssocket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP)) == -1 ){
        perror((char * )Ssocket);
        exit(1);
    }
<<<<<<< HEAD
    if(inet_aton(ip.c_str(),&server_addr.sin_addr) == 0){
    	fprintf(stderr, "inet_aton() failed\n");
	exit(1);
    }
=======

    if( inet_aton(ip.c_str(),&server_addr.sin_addr) == 0  ){
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

>>>>>>> 81a8730cb8c7fd203722eaacf4c6c636db2f747d
}

void UDPClient::echo(){
    int close = 0;
    int counter = 0;
    while(counter < 1){
        Send("Testing Message");
        Receive();
	counter++;
    }
    if( shutdown(Ssocket, SHUT_WR) == -1 ){
        perror("Socket Failed to close: Write\n");
        exit(1);
    }
    if( shutdown(Ssocket, SHUT_RD) == -1 ){
        perror("Socket Failed to close: Read\n");
        exit(1);
    }
}

void UDPClient::Send(string data){

    if(sendto(Ssocket, data.c_str() , data.size() , 0, (struct sockaddr * ) &server_addr, Slength ) == -1){
        perror("sendto");
        exit(1);
    }

}

void UDPClient::Receive(){

    char* buf = &Buffer[0];
    memset(buf, '\0', BufferLength);

    if( (ReceiveLength = recvfrom(Ssocket, buf, BufferLength,MSG_WAITALL,(struct sockaddr * ) &server_addr, &Slength )) == -1 ){
            perror("revfrom()");
            exit(1);
    }
    cout << "ReceivedLength: " << ReceiveLength << endl;
    cout << "Received packet from " << inet_ntoa(server_addr.sin_addr)  << ":" << ntohs(server_addr.sin_port) << endl;
    cout << "Data: ";
<<<<<<< HEAD
    for(int i = 0;/* i < ReceiveLength && */ i < BufferLength; i++){
=======
    for(int i = 0; i < BufferLength; i++){
>>>>>>> 81a8730cb8c7fd203722eaacf4c6c636db2f747d
        cout << Buffer[i];
    }
    cout << endl;

}
<<<<<<< HEAD
=======

void UDPClient::closeSocket(){
    if( close(Ssocket) == -1 ){
        perror("Socket Closing Error: ");
        exit(1);
    }
}
>>>>>>> 81a8730cb8c7fd203722eaacf4c6c636db2f747d
