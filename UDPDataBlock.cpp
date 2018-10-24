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
            data = "";
            count = 0;
        }
        datafile.close();
    }
}

void UDPData::append(string data){
    if(data.length() > BlockLength){
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
    nBlock.index = size();

    Blocks.push_back(nBlock);
}

struct DataBlock UDPData::operator[](int index){
    if(index >= Blocks.size()){
        cout << "Array index out of bound!" << endl;
        exit(0);
    }
    return Blocks[index];
}

string UDPData::toUDP(int index){
    string dString = "";
    unsigned int temp = Blocks[index].index;
    for(int i = 0; i < 4; i++){
        dString = (char)(temp % 256 ) + dString;
        temp = temp >> 8;
    }
    dString += Blocks[index].data;
    return dString;
}

void UDPData::fromUDP(string block){
    //Preconditions: assumes the receivers block vector is the same length as the senders
    
    struct DataBlock incomming;
    //getting index
    string strIndex = block.substr(0,4);
    incomming.index =   ((unsigned int)strIndex[0] << 24 )+
                        ((unsigned int)strIndex[1] << 16 )+
                        ((unsigned int)strIndex[2] << 8  )+
                        ((unsigned int)strIndex[3]);
    //getting data from data packet
    incomming.data = block.substr(4,block.size());
    //putting block in place
    Blocks[incomming.index] = incomming;
}

int UDPData::size(){
    return Blocks.size();
}

void UDPData::resizeTo(int nLen){
    struct DataBlock blank;
    blank.index = 0;
    blank.data = "";
    Blocks.resize(nLen,blank);
}