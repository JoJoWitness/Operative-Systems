#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#ifdef __linux__
    #include "unix.h"
#elif _WIN32
    #include "windows.h"
#endif

int main(int argc, char* argv[])
{   
    #ifdef __linux__
        pipeFunction();
    #elif _WIN32
         pipeFunction(argc, argv);
    #endif
    return 0;
}