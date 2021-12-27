#include "Bitstream.h"

BitStream::BitStream(){
}
BitStream::BitStream(const char* file, char op){
    operation = op;
    index_buffer = MOST_SIGNIFICANT_BIT;
    if(operation == 'w'){ 
        buffer=0x00;
        ofs.open(file);
    }
    else if(operation == 'r'){             
        ifs.open(file);
        buffer = ifs.get();
    }else{
        cout << "error in the operation selected";
        throw(1);
    }
}
int BitStream::open(const char* file , char op ){
    operation = op;
    index_buffer = MOST_SIGNIFICANT_BIT;
    if(operation == 'w'){ 
        buffer=0x00;
        ofs.open(file);
        return 1;
    }
    else if(operation == 'r'){             
        ifs.open(file);
        buffer = ifs.get();
        return 1;
    }else{
        cout << "error in the operation selected";
        return 0;
    }
}

int BitStream::writeBit(uint8_t bit){
    if(operation != 'w'){
        cout << "operation is not defined as writing" << endl;
        return -1;
    } 
    if(index_buffer == 0x00){
        index_buffer = MOST_SIGNIFICANT_BIT;
        ofs << buffer;
        buffer = 0x0;
    }
    if(bit) buffer = buffer | index_buffer;
    index_buffer = index_buffer >> BIT;
    return 0;
}
int BitStream::writeInteger(uint16_t integer,uint32_t numBits){
    uint16_t index = 0x0001 << numBits;
    while(index != 0x00){
        int bit = index & integer;
        // cout << (bit?1:0x00);
        writeBit(bit?1:0);
        index = index >> 1;            
    }
    return 0;
}

int BitStream::readInteger(uint32_t numBits){
    int buf = 0x0;
    for(int i = 0; i < numBits; i++){
        int bit = readBit();
        // cout << buf << endl;
        if(bit == -2) return -2;
        buf = buf<<1 | bit;

    }
    return buf;
}
int BitStream::readBit(){
    if(operation != 'r'){
        cout << "operation is not defined as reading" << endl;
        return -1;
    }
    if(index_buffer == 0x00){
        index_buffer = MOST_SIGNIFICANT_BIT;
        buffer = ifs.get();
        if(ifs.eof()){
            cout << "cant read more bytes" << endl;
            ifs.close();
            return -2;
        }
        // cout << buffer;
    }
    int bit = buffer & index_buffer;
    index_buffer = index_buffer >> BIT;
    return bit?1:0; 
}
void BitStream::close(){
    if(operation == 'w'){
        ofs << buffer;
        // cout << buffer;
        ofs.close();
    }else{
        ifs.close();
    }
}

