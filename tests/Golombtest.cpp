#include "../headers/GolombCode.h"
int main(int argc, char** argv){
    char *p;
    int num;

    long conv = strtol(argv[1], &p, 10);
    GolombCode Gc_encode("out.txt",conv,'w');
    for(int i = 0 ; i < 5; i++){
        Gc_encode.encode_int('a');
    }
    Gc_encode.close();
    GolombCode Gc_decode("out.txt",conv,'r');
    // Gc_encode.close();
    for(int i = 0 ; i< 5 ; i++){
        Gc_decode.decode_int();

    }
    cout << "Done" << endl;

}