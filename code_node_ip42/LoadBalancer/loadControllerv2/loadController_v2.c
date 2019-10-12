#define _XOPEN_SOURCE 500

#include "ipclib.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAXBUF 15
#define STD_CTRL_TIME 60
#define STD_END_VAL 1.0

int main (int argc, char *argv[]) {
    
    /*
    //To Do
    //letters instead of numbers in the input
    //comments
    //read load form shm
    //semaphores: only read when not opened
    //controler to endValue until control time is over
    //output: usleep()
    //signal ctrl + c free shm and shut down semaphore
    */
    
    //sample input: ./loadController -t9999 -ev100
    
    int icontrolTime = STD_CTRL_TIME;
    float fendValue = STD_END_VAL;
    
    //handle the inputs correctly
    //check for more than 2 options
    //check option 1 and convert to int
    //ckeck option 1 and 2 and convert to int
    //if option is not valid - stop program
    //if option is valid but not sensible - set to standard
    
    //check for too many arguments
    if(argc > 3) {
        printf("[loadController]: too many arguments \n");
        return EXIT_FAILURE;
    }
    
    if(argc == 3) {
        if(argv[1][0] != '-') {
            printf("[loadController]: wrong argument \n");
            return EXIT_FAILURE;
        }
        if(argv[1][1] != 't') {
            printf("[loadController]: wrong argument \n");
            return EXIT_FAILURE;
        }
        if(argv[2][0] != '-') {
            printf("[loadController]: wrong argument \n");
            return EXIT_FAILURE;
        }
        if(argv[2][1] != 'e') {
            printf("[loadController]: wrong argument \n");
            return EXIT_FAILURE;
        }
        if(argv[2][2] != 'v') {
            printf("[loadController]: wrong argument \n");
            return EXIT_FAILURE;
        }
        if(argv[2][3] != '0') {
            printf("[loadController]: wrong argument \n");
            return EXIT_FAILURE;
        }
        if(argv[2][4] != '.') {
            printf("[loadController]: wrong argument \n");
            return EXIT_FAILURE;
        }
    }
    if (argc == 3) {
        if(((argv[1][0] == '-') && (argv[1][1] == 't'))) {
            int decimalPlaces = 0;
            icontrolTime = 0;
            for (int i = 2; i < 6; i++) {
                if (argv[1][i] == '\0') {
                    break;
                }
                decimalPlaces = decimalPlaces + 1;
            }
            if(decimalPlaces == 0) {
                icontrolTime = STD_CTRL_TIME; 
            }
            if(decimalPlaces == 1) {
                icontrolTime = (int)(argv[1][2] - 48);
            }
            if(decimalPlaces == 2) {
                icontrolTime = ((int)(argv[1][2] - 48) * 10) + (int)(argv[1][3] - 48);
            }
            if(decimalPlaces == 3) {
                icontrolTime = ((int)(argv[1][2] - 48) * 100) + ((int)(argv[1][3] - 48) * 10) + ((int)(argv[1][4] - 48));
            }
            if(decimalPlaces == 4) {
                icontrolTime = ((int)(argv[1][2] - 48) * 1000) + ((int)(argv[1][3] - 48) * 100) + ((int)(argv[1][4] - 48) * 10) + ((int)(argv[1][5] - 48));
            }
        }
        if(((argv[2][0] == '-') && (argv[2][1] == 'e') && (argv[2][2] == 'v'))) {
            if (argv[2][7] != '\0') {
                printf("[loadController]: envalue wrong format e.g. -ev0.83 \n");
                return EXIT_FAILURE;
            }
            fendValue = (((float)(argv[2][5] - 48))/10) + (((float)(argv[2][6] - 48))/100);
        }
    }
    else if (argc == 2) {
        printf("[loadController]: wrong number of arguments \n");
        return EXIT_FAILURE;
    }
    
    //check for valid boundaries
    if((fendValue <= 0)) fendValue = STD_END_VAL;
    if((icontrolTime <= 0)) icontrolTime = STD_CTRL_TIME;
    if((fendValue >= 1)) fendValue = STD_END_VAL;
    if((icontrolTime >= 1800)) icontrolTime = STD_CTRL_TIME;
        
    printf("[loadController]: icontrolTime = %i \n", icontrolTime);
    printf("[loadController]: fendValue = %f \n", fendValue);    
    
    key_t semkey = 1234;
    int semid = semaphore_init(semkey);
    
    key_t shmkey = 5678;
    int shmid = sharedmem_init(shmkey, 4);
    
    float *sharedcpuload = sharedmem_attach(shmid); 
    float currentAVGLoad;
    double sleepMms = 100.0;
    double currentControlTime = 0.0;
    time_t startController = time(NULL);
    time_t stopController = 0;
    _Bool runningController = 1;
    
    while(1) {
        usleep((int)sleepMms);
        semaphore_operation(semid, LOCK);
        currentAVGLoad = *sharedcpuload;
        semaphore_operation(semid, UNLOCK);
        if((int)(currentControlTime = difftime(stopController = time(NULL), startController)) <= icontrolTime) {
            if (((int)currentAVGLoad) < fendValue) {
                sleepMms = sleepMms - 1.0;
                if(sleepMms < 2.0) {
                    sleepMms = 2.0;
                }
            }
            if (((int)currentAVGLoad) >= fendValue) {
                sleepMms = sleepMms + 1.0;
            }
        }
        else if (runningController == 1) {
            printf("[loadController]: controll time over \n");
            printf("[loadController]: sleepMms = %f \n", sleepMms);
            runningController = 0;
        }
    }
    
    sharedmem_detach(sharedcpuload);
    return EXIT_SUCCESS;

}