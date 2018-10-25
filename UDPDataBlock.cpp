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
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "UDPDataBlock.hpp"

using namespace std;
UDPData::UDPData(){
    BlockLength = 508;//508 maximum number of character or bytes per block
    Blocks = vector<struct DataBlock>();
}

UDPData::UDPData(unsigned int blocklength){
    BlockLength = blockLength;
    Blocks = vector<struct DataBlock>();
}
UDPData::UDPData(unsigned int blockLength, int size){
    BlockLength = blockLength;
    Blocks = vector<struct DataBlock>();
    resizeTo(size);
}

void UDPData::parseFile(string filename){
    ofstream datafile;
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

    ifstream outfile;
    outfile.open(filename, ios::out | ios::binary);
    for(int i = 0; i < Blocks.size(); i++){
        outfile.write(Blocks[i].data, Blocks[i].data.size());
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

    struct DataBlock nBlock;
    nBlock.data = data;
    nBlock.index = Blocks.size();
    nBlock.Ack = false;
    nBlock.handshake = false;
    nBlock.terminate = false;

    Blocks.push_back(nBlock);
}

struct DataBlock& UDPData::operator[](int index){
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
    string sIndex = atoi(Block.index);
    if(sIndex.size() < 10){
        int dif = 10 - sIndex.size();
        sIndex = string('0',dif) + sIndex;
    }
    dString.push_back( sIndex);
    dString.push_back( Block.data);
    return dString;
}

struct DataBlock UDPData::fromUDP(string block, int size){
    //Preconditions: assumes the receivers block vector is the same length as the senders
    
    struct DataBlock incomming;
    incomming.terminate = (bool)(block[0] - '0');
    incomming.Ack = (bool)(block[1] - '0');
    incomming.handshake = (bool)(block[2] - '0');
    incomming.index = stoi( block.substring(3,13) );
    incomming.data = block.substring(14,size);
    return incomming;
}

int UDPData::size(){
    return Blocks.size();
}

unsigned int UDPData::getBlockSize(){
    return BlockLength;
}

void UDPData::resizeTo(int nLen){
    struct DataBlock blank;
    blank.index = 0;
    blank.data = "";
    blank.Ack = false;
    blank.handshake = false;
    blank.terminate = false;
    Blocks.resize(nLen,blank);
}