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
#include <fstream>
#include <stdlib.h>
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
    TimeInterval.tv_usec = 500;

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
    TimeInterval.tv_usec = 500;

    Ssocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if( Ssocket == -1 ){
        perror((char*)Ssocket);
        exit(1);
    }

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
    if(debugMode||verboseMode){
        cout << "Server is running..." << endl;
    }
    signal(SIGINT, terminateServer);
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(Ssocket, &rfds);
    int running = 1;
    while(running){
        int selRet = select(Ssocket+1, &rfds, NULL,NULL, &TimeInterval);
        if(debugMode||verboseMode){
            cout << "selRet value: " << selRet << endl;
        }
        if(selRet == -1){
            //error
            perror("bind");
            closeSocket();
            exit(1);
        }
        else if(selRet == 0){
            //timeout
            if(debugMode||verboseMode){
                cout << "Timeout occured..." << endl;
            }
            double duration;
            vector<Connections>::iterator itr;
            for(itr = Clients.begin(); itr != Clients.end(); ++itr){
                duration = (clock() - itr->lastSent) / (double) CLOCKS_PER_SEC;
                if(duration > 90){//checks if this connection excessed wait time
                    if(itr->tries < 5){
                        //resend data
                        itr->tries++;
                        Send( UDPData::toUDP( itr->toSend[itr->position] ) , itr->address, itr->Slen );
                        itr->lastSent = clock();
                        if(debugMode||verboseMode){
                            cout << "Resending to: " << inet_ntoa( itr->address.sin_addr)  << ":" << ntohs(itr->address.sin_port) << endl;
                        }
                    }else{
                        //terminate connection after five timeouts
                        if(debugMode||verboseMode){
                            cout << "Removing Client: " << inet_ntoa( itr->address.sin_addr)  << ":" << ntohs(itr->address.sin_port) << endl;
                        }
                        Clients.erase(itr);
                    }
                }
            }
        }else{
            if(debugMode||verboseMode){
                cout << "Receving data" << endl;
            }
            Receive();
            UDPDataBlock packet;
            bool newConnection = true;
            vector<Connections>::iterator itr;
            for(itr = Clients.begin(); itr != Clients.end(); ++itr){
                if(client_addr.sin_addr.s_addr  == itr->address.sin_addr.s_addr ){
                    newConnection = false;
                    packet = UDPData::fromUDP(Buffer, itr->PacketLength);
                    if(packet.Ack){
                        if(debugMode||verboseMode){
                            cout << "Ack Recevied. ";
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
                        Send( UDPData::toUDP( itr->toSend[itr->position]),itr->address, itr->Slen );
                        itr->lastSent = clock();
                    }else if(packet.handshake){
                        if(packet.Ack){
                            if(debugMode||verboseMode){
                                cout << "Sending handShale Ack" << endl;
                            }
                            Send( UDPData::toUDP( itr->toSend[itr->position]),itr->address, itr->Slen );
                            itr->lastSent = clock();
                        }else{
                            if(debugMode||verboseMode){
                                cout << "Resending handshake" << endl;
                            }
                            UDPDataBlock resend;
                            resend.index = itr->toSend.size();
                            char* temp = (char*)malloc( (char)(itr->PacketLength-13+1) );
                            memset(temp,'0',(itr->PacketLength - 13));
                            resend.data =  temp;
                            resend.Ack = true;
                            resend.handshake = true;
                            resend.terminate = false;
                            Send( UDPData::toUDP(resend) ,itr->address, itr->Slen );
                            itr->lastSent = clock();
                        }
                    }else if(packet.terminate){
                        if(debugMode||verboseMode){
                            cout << "Termination packet recevied. Termination connection" << endl;
                        }
                        Send( UDPData::toUDP(packet) ,itr->address , itr->Slen );
                        Clients.erase(itr);
                    }
                    break;
                }
            }
            if(newConnection){
                if(debugMode||verboseMode){
                    cout << "Creating new connection" << endl;
                }
                packet = UDPData::fromUDP(Buffer,BufferLength);
                //add to active list
                Connections n;
                n.PacketLength = packet.index;
                n.position = 0;
                n.address = client_addr;
                n.Slen = sizeof(client_addr);
                n.tries = 0;
                fileToUDP(n.toSend, n.PacketLength);
                char* temp = (char*)malloc( (n.PacketLength-13+1) * sizeof(char));
                memset(temp, '0',n.PacketLength -13 );
                packet.data = temp;
                packet.index = n.toSend.size();
                packet.Ack = true;
                packet.handshake = true;
                packet.terminate = false;
                Send( UDPData::toUDP(packet),n.address, n.Slen );
                Clients.push_back(n);
                if(DebugMode||verboseMode){
                    cout << "new client added" << endl;
                }
            }
        }
        //clear client_addr
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
    cout << Buffer << endl;
    if( ( receiveLength = recvfrom(Ssocket, buf, BufferLength, 0, (struct sockaddr *) &client_addr, &Slength ) ) == -1 ){
        perror("recvfrom()");
        close(Ssocket);
        exit(1);
    }
    //Buffer = buf;
    cout << Buffer << endl;
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
        cout << "Sending..." << endl;
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
    string temp = dataToServe;
    cout << temp.size() << endl;
    cout << len << endl;
    cout << temp.size() % len << endl;
    int dif = (int)(temp.size() % len);
    if(DebugMode || verboseMode){
        cout << "Dif: " << dif << endl;
        cout << len << endl;
    }
    if( dif != 0){
        temp = temp + string('0',len - dif);
    }
    for(int i = 1; i < temp.size(); i *= len){
        cout << "i: " << i << "  i+len-1: " << i+len-1 << endl;
        cBlocks.append( temp.substr(i-1, i + len-1) );
    }
}