#ifndef UDPDATABLOCK_HPP
#define UDPDATABLOCK_HPP

#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct DataBlock{
    string data;
    size_t hash;
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
        int size();

};

#endif //UDPDATABLOCK_HPP