/*
*   File: UDPServer.cpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPServer class Definition
*   Class: CS484
*/

//Socket headers
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <ctime>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <signal.h>

#include "UDPServer.hpp"
#include "UDPDataBlock.hpp"

using namespace std;

bool UDPServer::DebugMode = false;
bool UDPServer::verboseMode = false;
int UDPServer::Ssocket;

UDPServer::UDPServer(string filename){
    BufferLength = 512;
    Port = 65535;
    Buffer = string();
    Buffer.resize(BufferLength, '0');
    readFile(filename);
    Clients = vector<Connections>();
    TimeInterval.tv_sec = 0;
    TimeInterval.tv_usec = 500000;

    Ssocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if( Ssocket == -1 ){
        perror((char*)Ssocket);
        exit(1);
    }
    //bzero( &my_addr, sizeof(my_addr) );
    memset( (char*) &my_addr,0, sizeof(my_addr) );
    Slength = sizeof(client_addr);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(Port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(Ssocket, (struct sockaddr*)&my_addr, sizeof(my_addr) ) == -1)
    {
        perror("bind");
        exit(1);
    }
    if(DebugMode||verboseMode){
        cout << "Socket " << Ssocket << " was sucsessfully binded" << endl;
    }
}
UDPServer::UDPServer(string filename, int port){
    BufferLength = 512;
    Port = port;
    Buffer = string();
    Buffer.resize(BufferLength, '0');
    
    readFile(filename);

    Clients = vector<Connections>();
    TimeInterval.tv_sec = 0;
    TimeInterval.tv_usec = 500000;

    Ssocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if( Ssocket == -1 ){
        perror((char*)Ssocket);
        exit(1);
    }
    Slength = sizeof(client_addr);

    memset( (char*) &my_addr,0, sizeof(my_addr) );
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(Port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind(Ssocket, (struct sockaddr*)&my_addr, sizeof(my_addr) ) == -1)
    {
        perror("bind");
        exit(1);
    }
    if(DebugMode||verboseMode){
        cout << "Socket " << Ssocket << " was sucsessfully binded" << endl;
    }
    
}
void UDPServer::run(){
    if(DebugMode||verboseMode){
        cout << "Server is running..." << endl;
    }
    signal(SIGINT, terminateServer);
    fd_set rfds;
    FD_ZERO(&rfds);
    char * temp = nullptr;
    int running = 1;
    while(running){
        FD_SET(Ssocket, &rfds);
        TimeInterval.tv_sec = 0;
        TimeInterval.tv_usec = 500;
        int selRet = select(Ssocket+1, &rfds, NULL,NULL, &TimeInterval);
        //if(DebugMode||verboseMode){
        //    cout << "selRet value: " << selRet << endl;
        //}
        if(selRet == -1){
            //error
            perror("bind");
            closeSocket();
            exit(1);
        }
        else if(selRet == 0){
            //timeout
            //if(DebugMode||verboseMode){
                //cout << "Timeout occured..." << endl;
            //}
            double duration;
            vector<Connections>::iterator itr;
            for(itr = Clients.begin(); itr != Clients.end(); ++itr){
                duration = (double)(clock() - itr->lastSent) / CLOCKS_PER_SEC;
                //cout << duration << endl;
                //if(DebugMode || verboseMode){
                    //printf("Duration: %f\n", duration);
                //}
                if(duration > 5){//checks if this connection excessed wait time
                    if(itr->tries < 5){
                        //resend data
                        itr->tries++;
                        if(DebugMode||verboseMode){
                            cout << "Resending to: " << inet_ntoa( itr->address.sin_addr)  << ":" << ntohs(itr->address.sin_port) << endl;
                        }
                        Send( UDPData::toUDP( itr->toSend[itr->position] ) , itr->address, itr->Slen );
                        itr->lastSent = clock();

                    }else{
                        //terminate connection after five timeouts
                        if(DebugMode||verboseMode){
                            cout << "Removing Client: " << inet_ntoa( itr->address.sin_addr)  << ":" << ntohs(itr->address.sin_port) << endl;
                        }
                        itr = Clients.erase(itr);
                    }
                }
            }
        }else{
            if(DebugMode||verboseMode){
                cout << "Receving data" << endl;
            }
            Receive();
            UDPDataBlock packet;
            bool newConnection = true;
            vector<Connections>::iterator itr;
            for(itr = Clients.begin(); itr != Clients.end(); ++itr){
                if(client_addr.sin_addr.s_addr  == itr->address.sin_addr.s_addr 
                    && client_addr.sin_port == itr->address.sin_port ){
                    newConnection = false;
                    packet = UDPData::fromUDP(Buffer, itr->PacketLength);
                    if(packet.terminate){
                        if(DebugMode||verboseMode){
                            cout << "Termination packet recevied. Termination connection" << endl;
                        }
                        temp = (char*)malloc( (char)(itr->PacketLength-13+1) );
                        memset(temp,'0',(itr->PacketLength - 13));
                        UDPData::makepacket(packet, temp, 0,false, false, true);
                        Send( UDPData::toUDP(packet) ,itr->address , itr->Slen );
                        free(temp);
                        cout << "erasing finised clients" << endl;
                        itr = Clients.erase(itr);
                        break;
                    }
                    else if(packet.handshake){
                        if(packet.Ack){
                            if(DebugMode||verboseMode){
                                cout << "Sending handShake Ack" << endl;
                            }
                            Send( UDPData::toUDP( itr->toSend[itr->position]),itr->address, itr->Slen );
                            itr->lastSent = clock();
                        }else{
                            if(DebugMode||verboseMode){
                                cout << "Resending handshake" << endl;
                            }
                            temp = (char*)malloc( (itr->PacketLength-12) * sizeof(char) );
                            memset(temp,'0',(itr->PacketLength - 13));
                            UDPData::makepacket(packet, temp, itr->toSend.size(), true, true, false);
                            Send( UDPData::toUDP(packet) ,itr->address, itr->Slen );
                            free(temp);
                            itr->lastSent = clock();
                        }
                        break;
                    }
                    else if(packet.Ack){
                        if(packet.index < itr->toSend.size()){
                            if(DebugMode||verboseMode){
                                cout << "Ack Recevied." << endl;
                            }
                            if(packet.index == itr->position){
                                cout << "Sending next packet" << endl;
                                itr->position++;
                                itr->tries = 0;
                            }else{
                                cout << "Resending packet" << endl;
                                itr->position = packet.index;
                                itr->tries++;
                            }
                            if(itr->position >= itr->toSend.size()){
                                if(DebugMode||verboseMode){
                                    cout << "All data has been sent.\nTerminatiing connection" << endl;
                                }
                                temp = (char*)malloc( (itr->PacketLength-12) * sizeof(char) );
                                memset(temp,'0',(itr->PacketLength - 13));
                                UDPData::makepacket(packet, temp,0, false,false,true);
                                Send( UDPData::toUDP( packet ),itr->address, itr->Slen );
                                free(temp);
                                itr = Clients.erase(itr);
                                cout << "removed client" << endl;
                            }else{
                                cout << "Sending packet" << endl;
                                Send( UDPData::toUDP( itr->toSend[itr->position]),itr->address, itr->Slen );
                                itr->lastSent = clock();
                            }
                        }
                        else{
                            if(DebugMode||verboseMode){
                                cout << "All data has been sent.\nTerminatiing connection" << endl;
                            }
                            temp = (char*)malloc( (itr->PacketLength-12) * sizeof(char) );
                            memset(temp,'0',(itr->PacketLength - 13));
                            UDPData::makepacket(packet, temp,0, false,false,true);
                            Send( UDPData::toUDP( packet ),itr->address, itr->Slen );
                            free(temp);
                            itr = Clients.erase(itr);
                            cout << "removed client" << endl;
                        }
                        break;
                    }
                }
            }
            if(newConnection){
                //Establishing a new connection
                if(DebugMode||verboseMode){
                    cout << "Creating new connection" << endl;
                }
                packet = UDPData::fromUDP(Buffer,BufferLength);
                //add to active list
                Connections n;
                n.PacketLength = packet.index;
                n.position = 0;
                n.address = client_addr;
                n.address.sin_family = AF_INET;
                n.Slen = sizeof(client_addr);
                n.tries = 0;
                n.toSend = UDPData(packet.index);
                fileToUDP(n.toSend, n.PacketLength);
                temp = (char*)malloc( (n.PacketLength-12) * sizeof(char));
                memset(temp, '0',n.PacketLength -13 );
                UDPData::makepacket(packet, temp, n.toSend.size(), true, true, false);
                Send( UDPData::toUDP(packet),n.address, n.Slen );
                n.lastSent = clock();
                free(temp);
                Clients.push_back(n);
                if(DebugMode||verboseMode){
                    cout << "new client added" << endl;
                }
            }
        }
        //clear client_addr
        //cout << "clearing temp address" << endl;
        memset(&client_addr, 0, sizeof(sockaddr_in));
        
    }
}
/*
void UDPServer::echo(){
    /*
        Does a simple echo procedure
    /
    int close = 0;
    int counter = 0;
    while(counter < 1){
        Receive();
        string echo = Buffer;
        Send( echo );
        counter++;
    }
    closeSocket();
}*/

void UDPServer::Receive(){
    /*
        Receives incoming UDP data from socket
    */
    if(DebugMode || verboseMode) {cout << "Receiving..." << endl;}
    int receiveLength;
    char* buf = &Buffer[0];
    memset(buf, '0', BufferLength);
    if( ( receiveLength = recvfrom(Ssocket, buf, BufferLength, 0, (struct sockaddr *) &client_addr, &Slength ) ) == -1 ){
        perror("recvfrom()");
        close(Ssocket);
        exit(1);
    }
    //Buffer = buf;
    if(DebugMode || verboseMode){
        cout << "ReceivedLength: " << receiveLength << endl;
        cout << "Buffer Length: " << Buffer.size() << " | " << BufferLength << endl;
        cout << "Received packet from " << inet_ntoa(client_addr.sin_addr)  << ":" << ntohs(client_addr.sin_port) << endl;
        cout << "Data: " << Buffer << endl;
    }
}

void UDPServer::Send(string data, struct sockaddr_in client, socklen_t Clen){
    /*
        Send data to desired address
    */
   if(DebugMode || verboseMode){
        cout << "Sending to client: " << inet_ntoa(client.sin_addr)  << ":" << ntohs(client.sin_port) << endl;
        cout << "DataLength:" << data.size() << endl;
        cout << "Data:" << data << endl;
    }
    int sentlength;
    if ( (sentlength =  sendto(Ssocket, data.c_str() , data.size(), 0, (struct sockaddr*) &client, Clen)) == -1){
        perror("sendto()");
        close(Ssocket);
        exit(1);
    }
    if(DebugMode || verboseMode){cout << "sent Length:" << sentlength << endl;}

}
void UDPServer::closeSocket(){
    /*
        Properly closes the socket
    */
    if(DebugMode || verboseMode){ cout << "closing socket " << Ssocket << endl;}
    if (close(Ssocket) == -1){
        perror("Socket Closing Error: ");
        exit(1);
    }
    if(DebugMode || verboseMode){ cout << "Socket " << Ssocket  << " successfuly closed"<< endl;}
}
void UDPServer::terminateServer( int signum ){
    cout << "\nInterrupt signal (" << signum << ") received." << endl;

    closeSocket();
    exit(signum);
}

void UDPServer::readFile(string filename){
    cout << "reading file: " << filename << endl;
    ifstream datafile;
    datafile.open(filename, ifstream::in | ifstream::binary);
    if(datafile.is_open()){
        char C;
        string data = "";
        while(!datafile.fail()){
                C = datafile.get();
                data.push_back(C);
        }
        data.pop_back();
        cout << "closing file" << endl;
        dataToServe = data;
        //cout << dataToServe << endl;
        datafile.close();
    }else{
        cout << "failed to open file" << endl;
    }
}

void UDPServer::fileToUDP(UDPData &cBlocks,int len){
    cout << "in fileToUDP" << endl;
    int dataLen = len-13;
    string temp = dataToServe;
    int remdr = (int)(temp.size() % dataLen);
    if(DebugMode || verboseMode){
        cout << "Remainder: " << remdr << endl;
        cout << len << endl;
    }
    if( remdr != 0){
        int dif = len - remdr;
        for(int i = 0; i < dif; i++){
            temp.push_back('0');
        }
    }
    int div = temp.size() / dataLen;
    for(int i = 0; i < div ; i++){
        int start = i * dataLen;
        cBlocks.append( temp.substr(start,dataLen ) );
    }
}