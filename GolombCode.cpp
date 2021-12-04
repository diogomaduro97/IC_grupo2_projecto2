
#include <iostream>
#include <fstream>
#include "Bitstream.h"
#include <math.h>

using namespace std;

#define LEAST_SIGNIFICANT_BIT 0x01
#define MOST_SIGNIFICANT_BIT 0x80
#define BIT 1

class GolombCode{
    private:
        const char* file;
        uint32_t m;
        uint32_t rMax;
    public:
        GolombCode(const char* f, uint32_t mconst){
            file = f;
            m = mconst;
            rMax = log2(m-1);
        }
        void encode(const char* fileIn){
            ifstream ifs(file);
            BitStream bofs(fileIn,'w');
            string line;
            int count = 0;
            while(getline(ifs,line, '\0')){
                for(char x : line){
                    cout << (uint16_t)x;
                    uint16_t q = (uint16_t)(x/m);
                    cout << " : " <<(uint16_t)(x/m)<< " : " << x - (uint16_t)(x/m) * m  << endl;
                    uint16_t r = x-q*m;
                    for(int i=0; i<q; i++){
                        bofs.writeBit(0);
                    }
                 
                    bofs.writeBit(1);
                    bofs.writeInteger(r,rMax);
                }
            }

        }
        void decode(const char* fileOut,const char* fileEncoded){
            ofstream ofs(fileOut);
            BitStream bofs(fileEncoded,'r');
            int bit = bofs.readBit();
            int q = 0;
            int flag = 0;
            int r ;
            uint8_t c;
            cout << bit << endl;
            while(bit != -2){
                if(flag == 0){
                    bit?flag=1:q++;
                }
                if(flag == 1){    
                    r = bofs.readInteger(rMax+1);
                    cout << q << " : " << r << endl;
                    ofs << (uint8_t)(q*m+r);
                    q = 0;
                    flag = 0;
                }
                bit = bofs.readBit();                
                
            }

        }

};
int main(){
    GolombCode Gc("in.txt",8);
    Gc.encode("out2.txt");
    Gc.decode("out3.txt","out2.txt");
    cout << "Done" << endl;

}