#include <cmath>
#include <fstream>
#include <list>
#include <sstream>
#include <string>

/**
 * A class for managing the paging disk and both physical and virtual memory.
 */
class VMManager
{
public:
    /**
     * Initialize the storage based on the init file.
     * @param initFile a C-string representing the directory of the init file
     */
    VMManager(char* initFile);
    
    /**
     * Translate and write each virtual memory address in the input file to output-dp.txt.
     * @param inputFile a C-string representing the directory of the input file
     */
    void start(char* inputFile);
    
    /**
     * Deallocate and destroy the physical and virtual memory space.
     */
    ~VMManager();

private:
    /**
     * Fill the physical and the paging disk with zero.
     */
    void fillZero();
    
    /**
     * Mark frame 2 to frame 1024 as free.
     */
    void fillFreeFrames();
    
    /**
     * Initialize the segment tables according to the first line of the init file.
     * @param segmentLine the first line of the init file
     */
    void initST(std::string segmentLine);
    
    /**
     * Initialize the page tables according to the second line of the init file.
     * @param pageLine the second line of the init file
     */
    void initPT(std::string pageLine);
    
    /**
     * Copy an entire block from paging disk d[b] into the physical memory starting at pm[m].
     * @param b the targeted block in the paging disk
     * @param m the targeted location in the physical memory
     */
    void readBlock(int b, int m);
    
    /**
     * Translate a single virtual memory address. The details are in the source file.
     * @param address an int representing the virtual address
     * @return an int representing the physical memory address
     */
    int translateVMAddress(int address);
    
private:
    // Physical memory of 524288 bytes
    int* pm;
    
    // A paging disk
    int** d;
    
    // A linked list of free frames
    std::list<int> freeFrames;
};