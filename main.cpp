#include "VMManager.h"

/**
 * Begin the program by taking the files in this form: ./icsosvm {your_init_file} {your_input_file}.
 * @param argc the number of parameters passed in, should be only 3
 * @param argv an array of parameters passed in
 * @return 0
 */
int main(int argc, char** argv)
{
    if (argc == 3)
    {
        VMManager simulation(argv[1]);
        simulation.start(argv[2]);
    }
    return 0;
}