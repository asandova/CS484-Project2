/*
*   File: UDPDataBlock.hpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: Contains the UDPDataBlock class Definition
*   Class: CS484
*/
#ifndef UDPDATABLOCK_HPP
#define UDPDATABLOCK_HPP

#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct DataBlock{
    //DateBlock stores the check hash and data that is to be transmitted with UDP
    //Each block is to be capped at 512 bytes
    string data; // can be a maximum of 504 characters/bytes
    size_t hash; // size_t uses 8 bytes
};

class UDPData{

    private:
        size_t blockLength;
        vector<struct DataBlock> Blocks;
        hash<string> blockHasher;
    public:
        UDPData(size_t blockLength);
        void parseFile(string filename);
        void append(string data);
        struct DataBlock operator[](int index);
        string toUDP(size_t index);
        void fromUDP();
        int size();

};

#endif //UDPDATABLOCK_HPP