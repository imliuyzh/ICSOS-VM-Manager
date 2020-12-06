#include "VMManager.h"

VMManager::VMManager(char* initFile)
    : pm{new int[524288]}, d{new int*[1024]}
{
    init(initFile);
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

void VMManager::init(char* initFile)
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

void VMManager::readBlock(int b, int m)
{
    int pmIndex = m;
    for (int counter = 0; counter < 512; ++counter)
    {
        pm[pmIndex] = d[b][counter];
        ++pmIndex;
    }
}

void VMManager::writeBlock(int b, int m)
{
    int pmIndex = m;
    for (int counter = 0; counter < 512; ++counter)
    {
        d[b][counter] = pm[pmIndex];
        ++pmIndex;
    }
}

int VMManager::translateVMAddress(int address)
{
    int s = address >> 18, w = address & 0x1FF, p = (address >> 9) & 0x1FF, pw = address & 0x3FFFF;
    if (pw >= pm[2 * s])
    {
        return -1;
    }
    
    if (pm[(2 * s) + 1] < 0)
    {
        int f1 = freeFrames.front();
        freeFrames.pop_front();
        readBlock(std::abs(pm[(2 * s) + 1]), f1 * 512);
        pm[(2 * s) + 1] = f1;
    }

    if (pm[(pm[(2 * s) + 1] * 512) + p] < 0)
    {
        int f2 = freeFrames.front();
        freeFrames.pop_front();
        readBlock(std::abs(pm[(pm[(2 * s) + 1] * 512) + p]), f2 * 512);
        pm[(pm[(2 * s) + 1] * 512) + p] = f2;
    }
    return (pm[(pm[(2 * s) + 1] * 512) + p] * 512) + w;
}