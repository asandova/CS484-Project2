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
#include <limits>
#include <signal.h>

#include "UDPClient.hpp"

using namespace std;

bool UDPClient::DebugMode = false;
bool UDPClient::verboseMode = false;

UDPClient::UDPClient(string ip, int port){
    BufferLength = 512;
    Port = port;
    Buffer = string();
    Buffer.resize(BufferLength, '\0');

    Slength = sizeof(server_addr);

    waitTime.tv_usec = 500;
    tries = 0;

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr );

    if( (Ssocket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP)) == -1 ){
        perror((char * )Ssocket);
        exit(1);
    }

    if( inet_aton(ip.c_str(),&server_addr.sin_addr) == 0  ){
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

}

UDPClient(string ip, int port, int bufferLen){
    BufferLength = bufferLen;
    Port = port;
    Buffer = string();
    Buffer.resize(BufferLength, '\0');

    Slength = sizeof(server_addr);

    waitTime.tv_usec = 500;
    tries = 0;

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr );

    if( (Ssocket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP)) == -1 ){
        perror((char * )Ssocket);
        exit(1);
    }

    if( inet_aton(ip.c_str(),&server_addr.sin_addr) == 0  ){
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
}

void UDPClient::run(){
    signal(SIGINT, terminateClient );
    unsigned int position = 0;
    unsigned int totalPackets = numeric_limits<unsigned int>::max(); 
    struct DataBlock packet;

    //constructing connection request
    packet.data = string('\0',512-13);
    packet.index = bufferLength;
    packet.Ack = false;
    packet.handshake = true;
    packet.terminate = false;
    //sending connction request
    send( UDPData::toUDP(packet));
    lastSent = clock();

    //handshake loop
    while(position < totalPackets){
        //checking for incomming packets
        int selRet = select(Ssocket+1, &rfds, NULL,NULL, &TimeInterval);
        if(tries > 5){
            cout << "failed to connect to server.\nExiting.." << endl; 
            break;
        }
        if(selRet == -1){
            //error
            perror("bind");
            closeSocket();
            exit(1);
        }
        else if(selRet == 0){
            //timeout
            if(tries > 5){
                cout << "Failed to connect. Shuting down" << endl;
                closeSocket();
                exit(1);
            }
            double duration;
            duration = (clock() - lastSent) / (double) CLOCKS_PER_SEC;
            if(duration > 90){
                send( UDPData::toUDP(packet));
                lastSent = clock();
                tries++;
            }

        }
        else{
            //Received a packet
            Receive();
            packet = UDPData::fromUDP(Buffer,BufferLength);
            if(packet.handshake){
                //received a handshake packet
                if(packet.Ack){
                    //sending clients second handshake packet
                    //sending the server ack for expected file length
                    receivedData = UDPData(bufferLength, packet.Index );
                    totalPackets = packet.Index;
                    packet.data = string('\0',bufferLenght-13);
                    packet.index = 0;
                    packet.Ack = true;
                    packet.handshake = true;
                    packet.terminate = false;
                    send( UDPData::toUDP(packet));
                    tries= 0;
                    lastSent = clock();
                }else{
                    //resending start connection packet
                    packet.data = string('\0',512-13);
                    packet.index = bufferLength;
                    packet.Ack = false;
                    packet.handshake = true;
                    packet.terminate = false;
                    send( UDPData::toUDP(packet));
                    tries++;
                    lastSent = clock();
                }
            }
            else{
                //receive data packet
                packet = UDPData::fromUDP(Buffer, BufferLength);
                if(packet.index == position){
                    receivedData[packet.position] = packet;
                    position++;

                }
                packet.data = string("\0",bufferLenght-13);
                packet.Ack = true;
                packet.handshake = false;
                packet.terminate = false;
                packet.index = position;
                Send(UDPData::toUDP(packet) );
            }          
        }
    }
    closeSocket();
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
    closeSocket();
}

void UDPClient::Send(string data){

    if(debugMode || verboseMode){
        cout << "Sending: " << data << endl;
    }
    if(sendto(Ssocket, data.c_str() , data.size() , 0, (struct sockaddr * ) &server_addr, Slength ) == -1){
        perror("sendto");
        exit(1);
    }

}

void UDPClient::Receive(){

    if(debugMode || verboseMode) {cout << "Receiving..." << endl;}
    char* buf = Buffer;
    memset(buf, '\0', BufferLength);
    if( (ReceiveLength = recvfrom(Ssocket, buf, BufferLength,MSG_WAITALL,(struct sockaddr * ) &server_addr, &Slength )) == -1 ){
            perror("revfrom()");
            exit(1);
    }
    if(debugMode || verboseMode){
        cout << "ReceivedLength: " << ReceiveLength << endl;
        cout << "Received packet from " << inet_ntoa(server_addr.sin_addr)  << ":" << ntohs(server_addr.sin_port) << endl;
        cout << "Data: ";
        for(int i = 0; i < BufferLength; i++){
            cout << Buffer[i];
        }
        cout << endl;
    }
}

void UDPClient::closeSocket(){
    if( close(Ssocket) == -1 ){
        perror("Socket Closing Error: ");
        exit(1);
    }
}
void UDPClient::terminateClient(){
    //used to terminate client via ctrl c
    closeSocket();
    exit(1);
}