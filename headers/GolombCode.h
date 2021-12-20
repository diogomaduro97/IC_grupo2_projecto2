
#include <iostream>
#include <fstream>
#include "Bitstream.h"
#include <math.h>
class GolombCode{
    private:
        const char* file;
        uint32_t m;
        uint32_t rMax;
        BitStream bofs,bifs;
        ifstream ifs,ofs;
    public:
        GolombCode(const char* f, uint32_t mconst, char op);
        void encode_file(const char* fileIn);
        void encode_char(uint32_t x);
        void decode_text(const char* fileOut);
        uint32_t decode_int();

};