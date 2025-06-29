#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef __linux__
    #include "unix.h"
#elif _WIN32
    #include "windows.h"
#endif


int main()
{   
    pipeFunction();
    return 0;
}