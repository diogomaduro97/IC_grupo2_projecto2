#include "../headers/GolombCode.h"
int main(int argc, char** argv){
    char *p;
    int num;

    long conv = strtol(argv[1], &p, 10);
    GolombCode Gc("in.txt",conv);
    Gc.encode_file("out2.txt");
    Gc.decode_text("out3.txt","out2.txt");
    cout << "Done" << endl;

}