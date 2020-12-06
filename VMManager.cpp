// Please read the comments in the header file to understand each method.
#include "VMManager.h"

VMManager::VMManager(char* initFile)
    : pm{new int[524288]}, d{new int*[1024]}
{
    std::fstream initFileStream{initFile, std::ios::in};
    std::string segmentLine = "", pageLine = "";
    std::getline(initFileStream, segmentLine);
    std::getline(initFileStream, pageLine);
    initFileStream.close();
    
    fillZero();
    fillFreeFrames();

    initST(segmentLine);
    initPT(pageLine);
}

void VMManager::start(char* inputFile)
{
    std::fstream inputFileStream{inputFile, std::ios::in},
        outputFileStream{"output-dp.txt", std::ios::out|std::ios::trunc};
    int address = 0;
    while (inputFileStream >> address)
    {
        outputFileStream << translateVMAddress(address) << " ";
    }
    outputFileStream.close();
    inputFileStream.close();
}

VMManager::~VMManager()
{
    for (int blockCounter = 0; blockCounter < 1024; ++blockCounter)
    {
        delete[] d[blockCounter];
    }
    delete[] d;
    delete[] pm;
}

void VMManager::fillZero()
{
    for (int memoryCounter = 0; memoryCounter < 524288; ++memoryCounter)
    {
        pm[memoryCounter] = 0;
    }

    for (int blockCounter = 0; blockCounter < 1024; ++blockCounter)
    {
        d[blockCounter] = new int[512];
        for (int pageCounter = 0; pageCounter < 512; ++pageCounter)
        {
            d[blockCounter][pageCounter] = 0;
        }
    }
}

void VMManager::fillFreeFrames()
{
    for (int frameCounter = 2; frameCounter < 1024; ++frameCounter)
    {
        freeFrames.push_back(frameCounter);
    }
}

void VMManager::initST(std::string segmentLine)
{
    int si = 0, zi = 0, fi = 0;		// PT of segment si resides in frame fi. and the length of segment si is zi
    std::stringstream segmentLineStream(segmentLine);
    while (segmentLineStream >> si >> zi >> fi)
    {
        pm[2 * si] = zi;
        pm[(2 * si) + 1] = fi;
        
        if (fi > 0)
        {
            freeFrames.remove(fi);
        }
    }
}

void VMManager::initPT(std::string pageLine)
{
    int sj = 0, pj = 0, fj = 0;		// the page pj of segment sj resides in frame fj
    std::stringstream pageLineStream(pageLine);
    while (pageLineStream >> sj >> pj >> fj)
    {
        int segmentPT = pm[(2 * sj) + 1];
        
        if (segmentPT < 0)
        {
            d[std::abs(segmentPT)][pj] = fj;
        }
        else
        {
            pm[(segmentPT * 512) + pj] = fj;
        }

        if (fj > 0)
        {
            freeFrames.remove(fj);
        }
    }
}

void VMManager::readBlock(int b, int m)
{
    int pmIndex = m;
    for (int counter = 0; counter < 512; ++counter)
    {
        pm[pmIndex] = d[b][counter];
        ++pmIndex;
    }
}

/*
 * 1. PM[2s] refers to the size field of segment s and PM[2s+1] refers to the frame number, fi, of the PT of segment s.
 * 		If pw ≥ PM[2s], then return -1 (error)
 * 		If PM[2s + 1] < 0, then
 * 			Allocate free frame f1 using list of free frames
 * 			Update list of free frames
 * 			Read disk block b = |PM[2s + 1]| into PM staring at location f1*512
 * 			PM[2s + 1] = f1
 * 2. To find the starting address of the PT, the frame number is multiplied by the frame size: PM[2s+1]*512
 * 3. Adding the page number p to the starting address of the PT yields the PT entry of page p: PM[2s+1]*512+p
 * 4. PM[PM[2s+1]*512+p] then contains the frame number, fj, of the corresponding page p.
 * 		If PM[PM[2s + 1]*512 + p] < 0
 * 			Allocate free frame f2 using list of free frames
 * 			Update list of free frames
 * 			Read disk block b = |PM[PM[2s + 1]*512 + p]| into PM staring at location f2*512
 * 			PM[PM[2s + 1]*512 + p] = f2
 * 5. To find the starting address of page p, the frame number is again multiplied by the frame size: PM[PM[2s+1]*512+p]*512. 
 * Adding the offset w to the starting address of page p yields the final PA: PM[PM[2s+1]*512+p]*512+w.
 */
int VMManager::translateVMAddress(int address)
{
    int s = address >> 18, w = address & 0x1FF, p = (address >> 9) & 0x1FF, pw = address & 0x3FFFF;
    if (pw >= pm[2 * s])	// VA is outside of the segment boundary
    {
        return -1;
    }
    
    if (pm[(2 * s) + 1] < 0)	// page fault: PT is not resident
    {
        int f1 = freeFrames.front();
        freeFrames.pop_front();
        readBlock(std::abs(pm[(2 * s) + 1]), f1 * 512);
        pm[(2 * s) + 1] = f1;
    }

    if (pm[(pm[(2 * s) + 1] * 512) + p] < 0)	// page fault: page is not resident
    {
        int f2 = freeFrames.front();
        freeFrames.pop_front();
        readBlock(std::abs(pm[(pm[(2 * s) + 1] * 512) + p]), f2 * 512);
        pm[(pm[(2 * s) + 1] * 512) + p] = f2;
    }
    return (pm[(pm[(2 * s) + 1] * 512) + p] * 512) + w;
}