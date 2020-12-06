#include "VMManager.h"

int main(int argc, char** argv)
{
    if (argc == 3)
    {
        VMManager simulation(argv[1]);
        simulation.start(argv[2]);
    }
    return 0;
}