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
        BitStream(const char* file, char op){
            operation = op;
            if(operation == 'w'){ 
                index_buffer = LEAST_SIGNIFICANT_BIT;
                buffer=0x00;
                ofs.open(file);
            }
            else if(operation == 'r'){ 
                index_buffer = MOST_SIGNIFICANT_BIT;            
                ifs.open(file);
                buffer = ifs.get();
            }else{
                cout << "error in the operation selected";
            }
        }

        int writeBit(uint8_t bit){
            if(operation != 'w'){
                cout << "operation is not defined as writing" << endl;
                return -1;
            } 
            if(index_buffer == 0x00){
                index_buffer = LEAST_SIGNIFICANT_BIT;
                ofs << buffer;
                buffer = 0x0;
            }
            buffer = buffer << 1 | bit;
            index_buffer = index_buffer << BIT;
            return 0;
        }
        int readBit(){
            if(operation != 'r'){
                cout << "operation is not defined as reading" << endl;
                return -1;
            }
            if(index_buffer == 0x00){
                index_buffer = MOST_SIGNIFICANT_BIT;
                buffer = ifs.get();
                // cout << buffer;
            }
            int bit = buffer & index_buffer;
            index_buffer = index_buffer >> BIT;
            if(bit) return 1;
            else return 0; 
        }
        void close(){
            if(operation == 'w'){
                ofs << buffer;
                // cout << buffer;
                ofs.close();
            }else{
                ifs.close();
            }
        }
};

int main(){
    BitStream obts("out.txt", 'w');
    BitStream ibts("in.txt", 'r');
    for(int i = 0; i< 500; i++){
        int bit = ibts.readBit();
        obts.writeBit(bit);
    }
    ibts.close();
    obts.close();
}