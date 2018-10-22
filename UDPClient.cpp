#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <vector>

#include "UDPClient.hpp"


UDPClient::UDPClient(string ip, int port){
    Port = port;
    Buffer = vector<char>();
    Buffer.reserve(BufferLength);

    Slength = sizeof(server_addr);

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr );

    if( (Ssocket = socket(AF_INET, SOCK_DGRAM,0)) == -1 ){
        perror((char * )Ssocket);
        exit(1);
    }

}

void UDPClient::echo(){
    int close = 0;
    while(!close){
        Send();
        Receive();
    }
}

void UDPClient::Send(){

    if(sendto(Ssocket, "message", strlen("message") , 0, (struct sockaddr * ) &server_addr, Slength ) == -1){
        perror("sendto");
        exit(1);
    }

}

void UDPClient::Receive(){

    char* buf = &Buffer[0];
    memset(buf, '\0', BufferLength);

    if(recvfrom(Ssocket, buf, BufferLength,0,(struct sockaddr * ) &server_addr, &Slength ) == -1 ){
            perror("revfrom()");
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