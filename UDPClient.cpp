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
#include <netdb.h>
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

UDPClient::UDPClient(string ip, int port, bool useHostName){
    //Constructor: 
    //IP: String - Is the servers ip address or host name
    //port: int - is the lisenting port for the server
    //useHostname: bool - is the flag the tell the server a host name has been passed in
    BufferLength = 512; // default packet length
    Port = port;
    Buffer = string();// setting the input buffer
    Buffer.resize(BufferLength, '0');

    Slength = sizeof(server_addr);

    //setting the wait time for who long to wait for in comming packets
    waitTime.tv_sec = 0;
    waitTime.tv_usec = 500; //set to 500 microseconds
    
    tries = 0; //the number of attepts the client has sent to the serve 

    //initializing the server address variable
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);

    //checking if iseHostname flag is set
    if(useHostName){
        //if True it resolves the hostname to a ip address
        cout << "resolving hostname: " << ip << endl;
        string hIP = getHostIp(ip);
        cout << "resolved hostname to: " << hIP << endl;
        if( hIP.empty() ){
            perror("Hostname resolution failed.");
            exit(1);
        }
        inet_pton(AF_INET, hIP.c_str(), &server_addr.sin_addr );
        if( inet_aton(hIP.c_str(),&server_addr.sin_addr) == 0  ){
            fprintf(stderr, "inet_aton() failed\n");
            exit(1);
        }
    }else{
        //if false: it translate the ip string to a useable ip struct
        cout << "IP: " << ip << endl;
        inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr );
        if( inet_aton(ip.c_str(),&server_addr.sin_addr) == 0  ){
            fprintf(stderr, "inet_aton() failed\n");
            exit(1);
        }

    }
    //bind the socket to the defined port
    if( (Ssocket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP)) == -1 ){
        //if binding failed it will display a error message and exit the program
        perror((char * )Ssocket);
        exit(1);
    }
}

UDPClient::UDPClient(string ip, int port, unsigned int bufferLen, bool useHostName){
    //Constructor: 
    //IP: String - Is the servers ip address or host name
    //port: int - is the lisenting port for the server
    //bufferLen: unsigned int - is the packet length the server want to recieve during data transfer
    //useHostname: bool - is the flag the tell the server a host name has been passed in
    BufferLength = bufferLen;
    Port = port;
    Buffer = string();
    Buffer.resize(BufferLength, '0');

    Slength = sizeof(server_addr);

    //setting the wait time for who long to wait for in comming packets
    waitTime.tv_sec = 0;
    waitTime.tv_usec = 500;
    tries = 0;

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(Port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr );

    //checking if iseHostname flag is set
    if(useHostName){
        //if True it resolves the hostname to a ip address
        cout << "resolving hostname: " << ip << endl;
        string hIP = getHostIp(ip);
        cout << "resolved hostname to: " << hIP << endl;
        if( hIP.empty() ){
            perror("Hostname resolution failed.");
            exit(1);
        }
        inet_pton(AF_INET, hIP.c_str(), &server_addr.sin_addr );
        if( inet_aton(hIP.c_str(),&server_addr.sin_addr) == 0  ){
            fprintf(stderr, "inet_aton() failed\n");
            exit(1);
        }
    }else{
        //if false: it translate the ip string to a useable ip struct
        cout << "IP: " << ip << endl;
        inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr );
        if( inet_aton(ip.c_str(),&server_addr.sin_addr) == 0  ){
            fprintf(stderr, "inet_aton() failed\n");
            exit(1);
        }

    }
    //bind the socket to the defined port
    if( (Ssocket = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP)) == -1 ){
        //if binding failed it will display a error message and exit the program
        perror((char * )Ssocket);
        exit(1);
    }
    if(DebugMode||verboseMode){
        cout << "Socket " << Ssocket << " was sucsessfully binded" << endl;
    }
}

int UDPClient::run(){
    //Run: actuly runs the server
    if(DebugMode||verboseMode){
        cout << "Client is running..." << endl;
    }
    //capture the Control^C interupts
    signal(SIGINT, terminateClient );

    fd_set rfds;
    FD_ZERO(&rfds);
    //set flags
    bool terminate = false;
    bool transferComplete = false;
    //define buffer variable
    char* temp;
    //stores the last packet sent to the server
    UDPDataBlock lastPacket;

    unsigned int position = 0;
    //inital set the total number of packets to unsinged int limits
    //this will be over written one handshake with the server is done
    unsigned int totalPackets = numeric_limits<unsigned int>::max();

    UDPDataBlock packet;

    //constructing inital connection request
    UDPData::makepacket(packet, &temp, 509, BufferLength, false, true, false);
    //sending connction request
    Send( UDPData::toUDP(packet));
    lastSent = clock();
    lastPacket = packet;
    
    free(temp);

    //handshake loop
    while(position < totalPackets && !terminate){
        //in waiting loop

        //refreshes the wait time after each iteration
        waitTime.tv_sec = 0;
        waitTime.tv_usec = 500;
        FD_SET(Ssocket, &rfds);
        //checking for incomming packets
        if(verboseMode || DebugMode){
            cout << "waiting for responce." << endl;
        }
        //lisens for for incomming packets for a set time period
        //will set a flag when a packet is recieved or time has expired
        int selRet = select(Ssocket+1, &rfds, NULL, NULL, &waitTime);
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
            if(duration > 2){
                Send( UDPData::toUDP(lastPacket));
                lastSent = clock();
                tries++;
            }

        }
        else{
            //Received a packet
            if(verboseMode || DebugMode){
                cout << "Incomming packet" << endl;
            }
            Receive();
            packet = UDPData::fromUDP(Buffer,BufferLength);
            if(packet.terminate){
                cout << "termination packet received" << endl;
                break;
            }
            else if(packet.handshake){
                //received a handshake packet
                if(packet.Ack){
                    //sending clients second handshake packet
                    //sending the server ack for expected file length
                    receivedData = UDPData(BufferLength, packet.index );
                    totalPackets = packet.index;
                    UDPData::makepacket(packet, &temp, BufferLength-13, 0, true, true, false);
                    Send( UDPData::toUDP(packet));
                    lastSent = clock();
                    lastPacket = packet;
                    tries= 0;
                }else{
                    //resending start connection packet
                    UDPData::makepacket(packet, &temp, 509, BufferLength, false, true, false);
                    Send( UDPData::toUDP(packet) );
                    lastSent = clock();
                    lastPacket = packet;
                    tries++;
                }
                free(temp);
                continue;
            }
            else {
                //receive data packet

                //tests if all data has been received
                if(position >= receivedData.size()){
                    UDPData::makepacket(packet, &temp, BufferLength-13,0, false, false, true);
                    transferComplete = true;
                }
                else{
                    //inserts the revicved packet in to the correct order
                    receivedData[packet.index] = packet;
                    position++;
                    //test if this packet is the last packet
                    if(position >= receivedData.size()){
                        //if true: send termination packet to server
                        transferComplete = true;
                        UDPData::makepacket(packet ,&temp, BufferLength-13, 0, false, false, true);
                    }else{
                        //if false: ack received packet and ask for next packet
                        UDPData::makepacket(packet ,&temp, BufferLength-13, position, true, false, false);
                    }
                }
                //sending packet
                Send(UDPData::toUDP(packet) );
                lastSent = clock();//setting time of last sent
                lastPacket = packet;//saveing sent packet
                free(temp);
            }     
           continue;    
        }
    }
    closeSocket();//closes socket
    cout << "Transfer Complete: " << transferComplete << endl;
    return transferComplete;
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
    //send the passed in data to the sever
    if(DebugMode || verboseMode){
        cout << "Sending: " << data << endl;
        cout << "DataLength:" << data.size() << endl;
        cout << "Data:" << data << endl;
    }
    int sendlen;
    if( (sendlen = sendto(Ssocket, data.c_str(), BufferLength, 0, (struct sockaddr * ) &server_addr, Slength )) == -1){
        perror("sendto");
        exit(1);
    }
    if(DebugMode || verboseMode){
        cout << "sent length: " << sendlen << endl;
    }
}

void UDPClient::Receive(){
    //receives a packet from the server and save it in buffer
    if(DebugMode || verboseMode) {cout << "Receiving..." << endl;}
    char* buf = &Buffer[0];
    memset(buf, '0', BufferLength);
    if( (ReceiveLength = recvfrom(Ssocket, buf, BufferLength, 0, (struct sockaddr * ) &server_addr, &Slength )) == -1 ){
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
    //saves completed transfer toa file if a given name
    receivedData.toFile(filename);
}

void UDPClient::closeSocket(){
    //close the socket properly
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
string UDPClient::getHostIp(string name){
    //preforms host name resolution
    //returns a string of the resolved ip address
    char * ip;
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
    if(  (he = gethostbyname(name.c_str()) ) == NULL ){
        perror("gethostbyname:");
        exit(1);
    }
    addr_list = (struct in_addr **)he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++){
        return string(inet_ntoa(*addr_list[i]));
    }
    return string();
}