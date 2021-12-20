#include "headers/GolombCode.h"


using namespace std;



GolombCode::GolombCode(const char* f, uint32_t mconst, char op){
    m = mconst;
    rMax = log2(m-1);
    if(op == 'w'){  
        // ifs.open(f1);
        bifs.open(f,'w');
    }else{
        bofs.open(f,'r');
        // ofs.open(f2);
    }
}
void GolombCode::encode_file(const char* fileIn){
    // BitStream bifs(fileIn,'w');
    int count = 0;
    while(!ifs.eof()){
        char x = ifs.get();
        // cout << (uint16_t)x;
        uint16_t q = (uint16_t)(x/m);
        // cout << " : " <<(uint16_t)(x/m)<< " : " << x - (uint16_t)(x/m) * m  << endl;
        uint16_t r = x-q*m;
        for(int i=0; i<q; i++){
            bifs.writeBit(0);
        }
        
        bifs.writeBit(1);
        bifs.writeInteger(r,rMax);
        
    }

}

void GolombCode::encode_char(uint32_t x){
    // BitStream bifs(file,'w');
    int count = 0;
    // cout << (uint16_t)x;
    uint32_t q = x/m;
    // cout << " : " <<(uint16_t)(x/m)<< " : " << x - (uint16_t)(x/m) * m  << endl;
    uint32_t r = x-q*m;
    for(int i=0; i<q; i++){
        bifs.writeBit(0);
        // cout << "0" ;
    }
    bifs.writeBit(1);
    // cout<< "1";
    bifs.writeInteger(r,rMax);
    // cout<< r<< endl;
}
void GolombCode::decode_text(const char* fileOut){
    ofstream ofs(fileOut);
    int bit = bofs.readBit();
    int q = 0;
    int flag = 0;
    int r ;
    uint8_t c;
    // cout << bit << endl;

    while(bit != -2){
        if(flag == 0){
            bit?flag=1:q++;
        }
        if(flag == 1){
            r = bofs.readInteger(rMax+1);
            if(r == -2) break;
            // cout << q << " : " << r << endl;
            ofs << (uint8_t)(q*m+r);
            q = 0;
            flag = 0;
        }
        bit = bofs.readBit();           
    }
}
uint32_t GolombCode::decode_int(){
    int bit = bofs.readBit();
    int q = 0;
    int flag = 0;
    int r ;
    // cout << bit << endl;

    while(bit != -2){
        if(flag == 0){
            bit?flag=1:q++;
        }
        if(flag == 1){
            r = bofs.readInteger(rMax+1);
            if(r == -2) break;
            // cout << q << " : " << r << endl;
            return (q*m+r);
        }
        bit = bofs.readBit();           
    }
    return 0;
}
