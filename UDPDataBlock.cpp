#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "UDPDataBlock.hpp"

using namespace std;

UDPData::UDPData(size_t blocklength){
    blockLength = blocklength;
    Blocks = vector<DataBlock>();
}

void UDPData::parseFile(string filename){
    ifstream datafile;
    datafile.open(filename, ios::in | ios::binary);
    if(!datafile.is_open()){
        cout << "File failed to open" << endl;
    }
    while(!datafile.fail()){
        char C;
        string data = "";
        int count = 0;
        while(count < blockLength){
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

void UDPData::append(string data){
    if(data.length() > blockLength){
        cout << "entered data is too large for current size" << endl;
    }
    else if(data.length() < blockLength){
        int fill = blockLength - data.length();
        for(int i = 0; i < fill; i++){
            data.push_back('\0');
        }
    }

    struct DataBlock nBlock;
    nBlock.data = data;
    nBlock.hash = blockHasher(data);

    Blocks.push_back(nBlock);
}

struct DataBlock UDPData::operator[](int index){
    if(index >= Blocks.size()){
        cout << "Array index out of bound!" << endl;
        exit(0);
    }
    return Blocks[index];
}

int UDPData::size(){
    return Blocks.size();
}
