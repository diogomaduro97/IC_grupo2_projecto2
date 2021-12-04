#include <iostream>
#include <fstream>
using namespace std;
#define LEAST_SIGNIFICANT_BIT 0x01
#define MOST_SIGNIFICANT_BIT 0x80
#define BIT 1
class BitStream{
    private:
        char buffer;
        char operation;
        uint8_t index_buffer;
        ofstream ofs;
        ifstream ifs;
    public:
        BitStream(const char* file, char op);
        int writeBit(uint8_t bit);
        int writeInteger(uint16_t integer,uint32_t numBits);
        int readInteger(uint32_t numBits);
        int readBit();
        void close();
};