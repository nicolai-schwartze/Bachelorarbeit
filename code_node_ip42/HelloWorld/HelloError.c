#define _XOPEN_SOURCE 500
#include <unistd.h> /* for fork */
#include <stdio.h>
#include <stdlib.h>


#include <signal.h>
#include <errno.h>

int checkPID (int pid) {
    int check = kill(pid, 0);
    if ((check == -1) && (errno == 3)) {
        return -1;
    }
    else {
        return 0;
    }
}

int main(int argc, char** argv) {
/*Spawn a child to run the program.*/
    printf("PID = 1: %i \n", checkPID(1));
    printf("error = %i \n", errno);
    printf("PID = 31415692: %i \n", checkPID(31415692));
    printf("error = %i \n", errno);
    
 return 0;
}