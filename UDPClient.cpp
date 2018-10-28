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
#include "UDPDataBlock.hpp"

using namespace std;

bool UDPClient::DebugMode = false;
bool UDPClient::verboseMode = false;
int UDPClient::Ssocket;

UDPClient::UDPClient(string ip, int port){
    BufferLength = 512;
    Port = port;
    Buffer = string();
    Buffer.resize(BufferLength, '0');

    Slength = sizeof(server_addr);

    waitTime.tv_sec = 0;
    waitTime.tv_usec = 500000;
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

UDPClient::UDPClient(string ip, int port, unsigned int bufferLen){
    BufferLength = bufferLen;
    Port = port;
    Buffer = string();
    Buffer.resize(BufferLength, '0');

    Slength = sizeof(server_addr);

    waitTime.tv_sec = 0;
    waitTime.tv_usec = 500000;
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
    if(DebugMode||verboseMode){
        cout << "Socket " << Ssocket << " was sucsessfully binded" << endl;
    }
}

void UDPClient::run(){
    if(DebugMode||verboseMode){
        cout << "Client is running..." << endl;
    }
    signal(SIGINT, terminateClient );
    fd_set rfds;
    FD_ZERO(&rfds);
    bool terminate = false;
    char* temp;

    unsigned int position = 0;
    unsigned int totalPackets = numeric_limits<unsigned int>::max(); 
    UDPDataBlock packet;

    //constructing connection request
    temp = (char*) malloc( 510 * sizeof(char) );
    memset(temp, '0', 509);
    packet.data = temp;
    temp = nullptr;
    packet.index = BufferLength;
    packet.Ack = false;
    packet.handshake = true;
    packet.terminate = false;
    //sending connction request
    Send( UDPData::toUDP(packet));
    lastSent = clock();

    //handshake loop
    while(position < totalPackets && !terminate){
        waitTime.tv_sec = 0;
        waitTime.tv_usec = 5000;
        FD_SET(Ssocket, &rfds);
        //checking for incomming packets
        if(verboseMode || DebugMode){
            cout << "waiting for responce." << endl;
        }
        int selRet = select(Ssocket+1, &rfds, NULL,NULL, &waitTime);
        if(selRet == -1){
            //error
            perror("bind");
            closeSocket();
            exit(1);
        }
        else if(selRet == 0){
            //timeout
            if(verboseMode || DebugMode){
            cout << "Time out occured" << endl;
            }
            if(tries > 5){
                cout << "Failed to connect. Shuting down" << endl;
                closeSocket();
                exit(1);
            }
            double duration;
            duration = (clock() - lastSent) / (double) CLOCKS_PER_SEC;
            if(duration > 1){
                Send( UDPData::toUDP(packet));
                lastSent = clock();
                tries++;
            }

        }
        else{
            //Received a packet
            Receive();
            packet = UDPData::fromUDP(Buffer,BufferLength);
            if(packet.terminate){
                break;
            }
            if(packet.handshake){
                //received a handshake packet
                if(packet.Ack){
                    //sending clients second handshake packet
                    //sending the server ack for expected file length
                    receivedData = UDPData(BufferLength, packet.index );
                    totalPackets = packet.index;
                    temp = (char*) malloc( BufferLength-13+1 * sizeof(char) );
                    memset(temp, '0', BufferLength-13);
                    packet.data = temp;
                    packet.index = 0;
                    packet.Ack = true;
                    packet.handshake = true;
                    packet.terminate = false;
                    Send( UDPData::toUDP(packet));
                    tries= 0;
                    lastSent = clock();
                }else{
                    //resending start connection packet
                    temp = (char*) malloc( 510 * sizeof(char) );
                    memset(temp, '0', 509);
                    packet.data = temp;
                    packet.index = BufferLength;
                    packet.Ack = false;
                    packet.handshake = true;
                    packet.terminate = false;
                    Send( UDPData::toUDP(packet) );
                    tries++;
                    lastSent = clock();
                }
            }
            else{
                //receive data packet
                packet = UDPData::fromUDP(Buffer, BufferLength);
                if(packet.index == position){
                    receivedData[packet.index] = packet;
                    position++;
                }
                packet.data = string("0",BufferLength-13);
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
/*
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
}*/

void UDPClient::Send(string data){

    if(DebugMode || verboseMode){
        cout << "Sending: " << data << endl;
        cout << "DataLength:" << data.size() << endl;
        cout << "Data:" << data << endl;
    }
    int sendlen;
    if( (sendlen = sendto(Ssocket, data.c_str() , BufferLength, 0, (struct sockaddr * ) &server_addr, Slength )) == -1){
        perror("sendto");
        exit(1);
    }
    if(DebugMode || verboseMode){
        cout << "sent length: " << sendlen << endl;
    }
}

void UDPClient::Receive(){

    if(DebugMode || verboseMode) {cout << "Receiving..." << endl;}
    char* buf = &Buffer[0];
    memset(buf, '0', BufferLength);
    if( (ReceiveLength = recvfrom(Ssocket, buf, BufferLength,MSG_WAITALL,(struct sockaddr * ) &server_addr, &Slength )) == -1 ){
            perror("revfrom()");
            close(Ssocket);
            exit(1);
    }
    if(DebugMode || verboseMode){
        cout << "ReceivedLength: " << ReceiveLength << endl;
        cout << "Received packet from " << inet_ntoa(server_addr.sin_addr)  << ":" << ntohs(server_addr.sin_port) << endl;
        cout << "Data: " << Buffer << endl;
    }
}

void UDPClient::save(string filename){
    receivedData.toFile(filename);
}

void UDPClient::closeSocket(){
    if( close(Ssocket) == -1 ){
        perror("Socket Closing Error: ");
        exit(1);
    }
}
void UDPClient::terminateClient(int signum){
    //used to terminate client via ctrl c
    cout << "\nInterrupt signal (" << signum << ") received." << endl;
    closeSocket();
    exit(signum );
}
