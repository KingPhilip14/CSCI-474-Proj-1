#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>
#include <fstream>


int main()
{
    int fds[2];
    pipe(fds);

    if(fork() == 0)
    {
        exit(0);
    }

    return 0;
}