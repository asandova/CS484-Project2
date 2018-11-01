/*
*   File: UDPDataBlock.hpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPDataBlock class Declaration
*   Class: CS484
*/
#ifndef UDPDATABLOCK_HPP
#define UDPDATABLOCK_HPP

#include <vector>
#include <string>

using namespace std;

typedef struct DataBlock{
    //DateBlock stores the block index and data that is to be transmitted with UDP
    //Each block is set at 512 bytes by default
    string data; // UDP playload
    unsigned int index; // used to tell the server where the client is at
    bool Ack; //used to determine if packet is a acknolagment packet
    bool handshake; //used when establishing a connection with a new client
    bool terminate; //used to tell the server or client of termination request
}UDPDataBlock; 

class UDPData{

    private:
        unsigned int BlockLength; // This is the length of each packet
        vector<UDPDataBlock> Blocks; //This is where all data that is either recevied or to be sent are stored
    public:
        UDPData(); //default
        UDPData(unsigned int blen);//sets the size of each block
        UDPData(unsigned int blen, int size); //sets the size of each block and amount of blocks to store
        //void parseFile(string filename);// converts a file to a chain of Blocks
        void toFile(string filename); // converts the chain of block to a file
        void append(string data);//add a block to the end of the chain
        UDPDataBlock& operator[](size_t index); //returns a block at position index
        int size(); // returns the total length of the chain
        unsigned int getBlockSize();
        static string toUDP( UDPDataBlock Block); //converts a dataBlock to a UDP Payload
        static UDPDataBlock fromUDP(string block, int size); //convers a UDP playload to dataBlock
        void resizeTo(int nlen); //expand the chain to length nlen
        static void makepacket(UDPDataBlock& pack,char** data, size_t datalen,unsigned index, bool Ack, bool handshake, bool terminate);
        static void removePadding(UDPDataBlock& block);
};

#endif //UDPDATABLOCK_HPP