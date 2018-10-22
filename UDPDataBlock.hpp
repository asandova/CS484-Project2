#ifdef UDPDATABLOCK_HPP
#define UDPDATABLOCK_HPP

#include <vector>
#include <string>

using namespace std;

struct hashBlock{
    string data;
    string hash;
};

class UDPData{

    private:
        int bufferLength;
        vector<struct hashBlock> Block;
    public:
        UDPData(int bufLength);
        void parseFile(string filename);
        void append(string data);
        void hash();
        friend struct hashBlock operator[](int index);
        int size();

};

#endif //UDPDATABLOCK_HPP