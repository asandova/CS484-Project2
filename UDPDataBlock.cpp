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
    BlockLength = 508;//508 maximum number of character or bytes per block
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

void UDPData::parseFile(string filename){
    ifstream datafile;
    datafile.open(filename, ios::in | ios::binary);
    if(!datafile.is_open()){
        cout << "File failed to open" << endl;
    }else{
        while(!datafile.fail()){
            char C;
            string data = "";
            int count = 0;
            while(count < BlockLength && !datafile.fail()){
                C = datafile.get();
                data.push_back(C);
                count++;
            }
            append(data);
        }
        datafile.close();
    }
}
void UDPData::toFile(string filename){

    ofstream outfile;
    outfile.open(filename, ios::out | ios::binary);
    for(int i = 0; i < Blocks.size(); i++){
        outfile.write( Blocks[i].data.c_str(), Blocks[i].data.size());
    }
    outfile.close();
}

void UDPData::append(string data){
    if(data.length() > BlockLength - 13){
        cout << "entered data is too large for current size" << endl;
    }
    else if(data.length() < BlockLength){
        int fill = BlockLength - data.length();
        for(int i = 0; i < fill; i++){
            data.push_back('\0');
        }
    }

    UDPDataBlock nBlock;
    nBlock.data = data;
    nBlock.index = Blocks.size();
    nBlock.Ack = false;
    nBlock.handshake = false;
    nBlock.terminate = false;

    Blocks.push_back(nBlock);
}

UDPDataBlock& UDPData::operator[](int index){
    if(index >= Blocks.size() || index < 0){
        cout << "Array index out of bound!" << endl;
        exit(0);
    }
    return Blocks[index];
}

string UDPData::toUDP(struct DataBlock Block){
    
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
    if(sIndex.size() < 10){
        int dif = 10 - sIndex.size();
        sIndex = string('0',dif) + sIndex;
    }
    dString.append( sIndex);
    dString.append( Block.data);
    return dString;
}

UDPDataBlock UDPData::fromUDP(string block, int size){
    //Preconditions: assumes the receivers block vector is the same length as the senders
    
    UDPDataBlock incomming;
    incomming.terminate = (bool)(block[0] - '0');
    incomming.Ack = (bool)(block[1] - '0');
    incomming.handshake = (bool)(block[2] - '0');
    incomming.index = stoi( block.substr(3,13) );
    incomming.data = block.substr(14,size);
    return incomming;
}

int UDPData::size(){
    return Blocks.size();
}

unsigned int UDPData::getBlockSize(){
    return BlockLength;
}

void UDPData::resizeTo(int nLen){
    UDPDataBlock blank;
    blank.index = 0;
    blank.data = "";
    blank.Ack = false;
    blank.handshake = false;
    blank.terminate = false;
    Blocks.resize(nLen,blank);
}