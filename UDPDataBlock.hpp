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
#include <unordered_map>

using namespace std;

struct DataBlock{
    //DateBlock stores the block index and data that is to be transmitted with UDP
    //Each block is to be capped at 512 bytes by default
    string data; // can be a maximum of 504 characters/bytes
    unsigned int index; // size_t uses 8 bytes
};

class UDPData{

    private:
        unsigned int BlockLength; // This is the length of each block
        vector<struct DataBlock> Blocks;
    public:
        UDPData(unsigned int blockLength);
        UDPData(unsigned int blockLength);
        void parseFile(string filename);
        void append(string data);
        struct DataBlock operator[](int index);
        int size();
        string toUDP(int index);
        void fromUDP(string block);

};

#endif //UDPDATABLOCK_HPP