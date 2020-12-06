#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

class VMManager
{
public:
    VMManager(char* initFile);
    void start(char* inputFile);
    ~VMManager();

private:
    void fillZero();
    void fillFreeFrames();
    void initST(std::string segmentLine);
    void initPT(std::string pageLine);
    void init(char* initFile);
    void readBlock(int b, int m);
    void writeBlock(int b, int m);
    int translateVMAddress(int address);
    
private:
    // Physical memory of 524288 bytes
    int* pm;
    
    // A paging disk
    int** d;
    
    // A linked list of free frames
    std::list<int> freeFrames;
};