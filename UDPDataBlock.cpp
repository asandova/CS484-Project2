/*
*   File: UDPDataBlock.cpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPDataBlock class Definition
*   Class: CS484
*/
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "UDPDataBlock.hpp"

using namespace std;

UDPData::UDPData(){
    //default constructor
    //set the total packet length to 512 bytes
    BlockLength = 512;
    Blocks = vector<UDPDataBlock>();
}

UDPData::UDPData(unsigned int blocklength){
    BlockLength = blocklength;
    Blocks = vector<UDPDataBlock>();
}
UDPData::UDPData(unsigned int blockLength, int size){
    BlockLength = blockLength;
    Blocks = vector<UDPDataBlock>();
    resizeTo(size);
}
void UDPData::toFile(string filename){
    //cout << "writing to file" << endl;
    ofstream outfile;
    outfile.open(filename, ios::out | ios::binary);
    for(int i = 0; i < Blocks.size(); i++){
        if(i+1 == Blocks.size()){
            removePadding(Blocks[i]);
        }
        outfile.write( Blocks[i].data.c_str(), Blocks[i].data.size());
    }
    outfile.close();
}

void UDPData::append(string data){
    if(data.length() > BlockLength-13){
        cout << BlockLength << endl;
        cout << "entered data is too large for current size" << endl;
    }
    else if(data.length() < BlockLength-13){
        int fill = (BlockLength-13) - data.length();
        for(int i = 0; i < fill; i++){
            data.push_back('0');
        }
    }

    UDPDataBlock nBlock;
    nBlock.data = data;
    nBlock.index = Blocks.size();
    nBlock.Ack = false;
    nBlock.handshake = false;
    nBlock.terminate = false;

    Blocks.push_back(nBlock);
    //cout << "block appended" << endl;
}

UDPDataBlock& UDPData::operator[](size_t index){
    if(index >= Blocks.size() || index < 0){
        cout << "Array index out of bound!" << endl;
        exit(0);
    }
    return Blocks[index];
}

string UDPData::toUDP(UDPDataBlock Block){
    
    string dString = "";
    if(Block.terminate ){
        dString.push_back('1');
    }else{
        dString.push_back('0');
    }
    if(Block.Ack){
        dString.push_back('1');
    }else{
        dString.push_back('0');
    }
    if(Block.handshake){
        dString.push_back('1');
    }else{
        dString.push_back('0');
    }
    stringstream hold;
    hold << Block.index;
    string sIndex = hold.str();
    cout << sIndex << endl;
    if(sIndex.size() < 10){
        int dif = 10 - sIndex.size();
        //cout << dif << endl;
        for(int i = 0; i < dif; i++){
            dString.push_back('0');
        }
    }
    dString = dString + sIndex;
    dString = dString + Block.data;
    return dString;
}

UDPDataBlock UDPData::fromUDP(string block, int size){
    //cout << "converting to Packet" << endl;
    UDPDataBlock incomming;
    
    incomming.terminate = (bool)(block[0] - '0');
    incomming.Ack = (bool)(block[1] - '0');
    incomming.handshake = (bool)(block[2] - '0');
    istringstream sindex(block.substr(3,10));
    cout << block.substr(3,10) << endl;
    sindex >> incomming.index;
    cout << incomming.index << endl;
    incomming.data = block.substr(13,size-13);
    //cout << "Packet converted" << endl;
    return incomming;
}

int UDPData::size(){
    //returns the number of block
    return Blocks.size();
}

unsigned int UDPData::getBlockSize(){
    //returns the the max size of the packect data block
    return BlockLength;
}

void UDPData::resizeTo(int nLen){
    //Resizes the UDPdata vector to size nLen 
    UDPDataBlock blank;
    blank.index = 0;
    blank.data = string('0',BlockLength-13);
    blank.Ack = false;
    blank.handshake = false;
    blank.terminate = false;
    Blocks.resize(nLen,blank);
}
void UDPData::makepacket(UDPDataBlock& pack, char** data, size_t datalen, unsigned index, bool Ack, bool handshake, bool terminate){
    //construct a UDPDataBlock from given arguments
    *data = (char*)malloc(datalen+1 * sizeof(char));
    memset(*data, '0', datalen);
    pack.data = string(*data);
    pack.index = index;
    pack.handshake = handshake;
    pack.Ack = Ack;
    pack.terminate = terminate;
}

void UDPData::removePadding(UDPDataBlock& block){
    //remove any added padding to the last packet
    for(int i = block.data.size()-1; i >= 0; i-- ){
        if(block.data[i] != '0'){
            block.data.resize(i);
            break;
        }
    }
}