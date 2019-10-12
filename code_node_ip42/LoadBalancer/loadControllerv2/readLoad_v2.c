#define _XOPEN_SOURCE 500

#include "ipclib.h"

#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>

#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

float readLoad(void) {
    
    float cpuLoadSum = 0.0;
    
    //open file
    FILE *loadAVG = NULL;
    loadAVG = fopen("/proc/loadavg", "r");
    //variable decleration
    char loadAVGContent [5] = "";
    for (int i = 0; i < 4; i++) {
        loadAVGContent[i] = getc(loadAVG);
    }
    loadAVGContent [4] = '\0';
    fclose(loadAVG);
    cpuLoadSum = (float)(loadAVGContent[0] - 48) + (((float)(loadAVGContent[2] - 48))/10) + (((float)(loadAVGContent[3] - 48))/100);
    return cpuLoadSum; 
}



//for testing

#define SHMSZ 4

int main(int argc, char *argv[]) {
    
    key_t semkey = 1234;
    int semid = semaphore_init(semkey);
    
    key_t shmkey = 5678;
    int shmid = sharedmem_init(shmkey, SHMSZ);
    
    float *loadaverage = sharedmem_attach(shmid);
    
    while(1) {
        semaphore_operation(semid, LOCK);
        *loadaverage = readLoad();
        sleep(3);
        printf("[readLoad]: load average = %f \n", *loadaverage);
        semaphore_operation(semid, UNLOCK);
        sleep(2);
    }
    

    return 0;
}