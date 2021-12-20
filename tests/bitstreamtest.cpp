#include "../headers/Bitstream.h"
int main(){
    BitStream obts("out.txt", 'w');
    BitStream ibts("in.txt", 'r');
    for(int i = 0; i< 500; i++){
        int bit = ibts.readBit();
        if(bit == -2) break;
        obts.writeBit(bit);
    }
    ibts.close();
    obts.close();
    uint16_t a = '1';
    cout << a << endl;
}