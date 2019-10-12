#define _XOPEN_SOURCE 500

#include "ipclib.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAXBUF 15
#define STD_CTRL_TIME 60
#define STD_END_VAL 70

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
    int iendValue = STD_END_VAL;
    
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
    
    //check for validity with only 1 argument
    //convert argument 1 to int
    if(argc == 2) {
        //argument is too long
        if(strlen(argv[1]) > 6) {
            printf("[loadController]: option 1 too long\n");
            return EXIT_FAILURE;
        }
        //argument is not -t or -ev
        if(((argv[1][0] != '-') && (argv[1][1] != 't')) || ((argv[1][0] != '-') && (argv[1][1] != 'e') && (argv[1][2] != 'v'))) {
            printf("[loadController]: option 1 not valid \n");
            return EXIT_FAILURE;
        }
        //convert argument -t from strin into int
        if(((argv[1][0] == '-') && (argv[1][1] == 't'))) {
            int decimalPlaces = 0;
            icontrolTime = 0;
            //count the dezimal places
            for (int i = 2; i < 6; i++) {
                if (argv[1][i] == '\0') {
                    break;
                }
                decimalPlaces = decimalPlaces + 1;
            }
            //if the input is only -t
            if(decimalPlaces == 0) {
                icontrolTime = STD_CTRL_TIME; 
            }
            //-t with one place
            if(decimalPlaces == 1) {
                icontrolTime = (int)(argv[1][2] - 48);
            }
            //-t with two places
            if(decimalPlaces == 2) {
                icontrolTime = ((int)(argv[1][2] - 48) * 10) + (int)(argv[1][3] - 48);
            }
            //-t with three places
            if(decimalPlaces == 3) {
                icontrolTime = ((int)(argv[1][2] - 48) * 100) + ((int)(argv[1][3] - 48) * 10) + ((int)(argv[1][4] - 48));
            }
            //-t with four places
            if(decimalPlaces == 4) {
                icontrolTime = ((int)(argv[1][2] - 48) * 1000) + ((int)(argv[1][3] - 48) * 100) + ((int)(argv[1][4] - 48) * 10) + ((int)(argv[1][5] - 48));
            }
        }
        //if the first argument is -ev
        if(((argv[1][0] == '-') && (argv[1][1] == 'e') && (argv[1][2] == 'v'))) {
            int decimalPlaces = 0;
            iendValue = 0;
            //count the decimal places
            for (int i = 3; i < 6; i++) {
                if (argv[1][i] == '\0') {
                    break;
                }
                decimalPlaces = decimalPlaces + 1;
            }
            //input is only -ev
            if(decimalPlaces == 0) {
                iendValue = STD_END_VAL; 
            }
            //-ev with one places
            if(decimalPlaces == 1) {
                iendValue = (int)(argv[1][3] - 48);
            }
            //-ev with two places
            if(decimalPlaces == 2) {
                iendValue = ((int)(argv[1][3] - 48) * 10) + (int)(argv[1][4] - 48);
            }
            //-ev with three places
            if(decimalPlaces == 3) {
                iendValue = ((int)(argv[1][3] - 48) * 100) + ((int)(argv[1][4] - 48) * 10) + ((int)(argv[1][5] - 48));
            }
        }
        
    }
    
    //check for validity with 2 arguments
    //convert argument 1 and 2 to int
    
    if(argc == 3) {
        //check for lenght of option 2
        if(strlen(argv[2]) > 6) {
            printf("[loadController]: option 2 too long\n");
            return EXIT_FAILURE;
        }
        //check for length of option 1
        if(strlen(argv[1]) > 6) {
            printf("[loadController]: option 1 too long\n");
            return EXIT_FAILURE;
        }
        //check for valid option 1
        if(((argv[1][0] != '-') && (argv[1][1] != 't')) || ((argv[1][0] != '-') && (argv[1][1] != 'e') && (argv[1][2] != 'v'))) {
            printf("[loadController]: option 1 not valid \n");
            return EXIT_FAILURE;
        }
        //check for valid option 2
        if(((argv[2][0] != '-') && (argv[2][1] != 't')) || ((argv[2][0] != '-') && (argv[2][1] != 'e') && (argv[2][2] != 'v'))) {
            printf("[loadController]: option 2 not valid \n");
            return EXIT_FAILURE;
        }
        
        //if option 1 is -t
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
        
        //if option 1 is -ev
        if(((argv[1][0] == '-') && (argv[1][1] == 'e') && (argv[1][2] == 'v'))) {
            int decimalPlaces = 0;
            iendValue = 0;
            for (int i = 3; i < 6; i++) {
                if (argv[1][i] == '\0') {
                    break;
                }
                decimalPlaces = decimalPlaces + 1;
            }
            if(decimalPlaces == 0) {
                iendValue = STD_END_VAL; 
            }
            if(decimalPlaces == 1) {
                iendValue = (int)(argv[1][3] - 48);
            }
            if(decimalPlaces == 2) {
                iendValue = ((int)(argv[1][3] - 48) * 10) + (int)(argv[1][4] - 48);
            }
            if(decimalPlaces == 3) {
                iendValue = ((int)(argv[1][3] - 48) * 100) + ((int)(argv[1][4] - 48) * 10) + ((int)(argv[1][5] - 48));
            }
        }
        
        //if option 2 is -t
        if(((argv[2][0] == '-') && (argv[2][1] == 't'))) {
            int decimalPlaces = 0;
            icontrolTime = 0;
            for (int i = 2; i < 6; i++) {
                if (argv[2][i] == '\0') {
                    break;
                }
                decimalPlaces = decimalPlaces + 1;
            }
            if(decimalPlaces == 0) {
                icontrolTime = STD_CTRL_TIME; 
            }
            if(decimalPlaces == 1) {
                icontrolTime = (int)(argv[2][2] - 48);
            }
            if(decimalPlaces == 2) {
                icontrolTime = ((int)(argv[2][2] - 48) * 10) + (int)(argv[2][3] - 48);
            }
            if(decimalPlaces == 3) {
                icontrolTime = ((int)(argv[2][2] - 48) * 100) + ((int)(argv[2][3] - 48) * 10) + ((int)(argv[2][4] - 48));
            }
            if(decimalPlaces == 4) {
                icontrolTime = ((int)(argv[2][2] - 48) * 1000) + ((int)(argv[2][3] - 48) * 100) + ((int)(argv[2][4] - 48) * 10) + ((int)(argv[2][5] - 48));
            }
        }
        
        //if option 2 is -ev
        if(((argv[2][0] == '-') && (argv[2][1] == 'e') && (argv[2][2] == 'v'))) {
            int decimalPlaces = 0;
            iendValue = 0;
            for (int i = 3; i < 6; i++) {
                if (argv[2][i] == '\0') {
                    break;
                }
                decimalPlaces = decimalPlaces + 1;
            }
            if(decimalPlaces == 0) {
                iendValue = STD_END_VAL; 
            }
            if(decimalPlaces == 1) {
                iendValue = (int)(argv[2][3] - 48);
            }
            if(decimalPlaces == 2) {
                iendValue = ((int)(argv[2][3] - 48) * 10) + (int)(argv[2][4] - 48);
            }
            if(decimalPlaces == 3) {
                iendValue = ((int)(argv[2][3] - 48) * 100) + ((int)(argv[2][4] - 48) * 10) + ((int)(argv[2][5] - 48));
            }
        }
    }
    
    //check for valid boundaries
    if((iendValue <= 0)) iendValue = STD_END_VAL;
    if((icontrolTime <= 0)) icontrolTime = STD_CTRL_TIME;
    if((iendValue >= 100)) iendValue = STD_END_VAL;
    if((icontrolTime >= 1800)) icontrolTime = STD_CTRL_TIME;
        
    printf("[loadController]: icontrolTime = %i \n", icontrolTime);
    printf("[loadController]: iendValue = %i \n", iendValue);    
    
    key_t semkey = 1234;
    int semid = semaphore_init(semkey);
    
    key_t shmkey = 5678;
    int shmid = sharedmem_init(shmkey, 4);
    
    float *sharedcpuload = sharedmem_attach(shmid); 
    float currentCpuLoad;
    double sleepMms = 1.0;
    double currentControlTime = 0.0;
    time_t startController = time(NULL);
    time_t stopController = 0;
    _Bool runningController = 1;
    
    while(1) {
        usleep((int)sleepMms);
        semaphore_operation(semid, LOCK);
        currentCpuLoad = *sharedcpuload;
        semaphore_operation(semid, UNLOCK);
        if((int)(currentControlTime = difftime(stopController = time(NULL), startController)) <= icontrolTime) {
            if (((int)currentCpuLoad) < iendValue) {
                sleepMms = sleepMms - 1.0;
                if(sleepMms < 2.0) {
                    sleepMms = 2.0;
                }
            }
            if (((int)currentCpuLoad) >= iendValue) {
                sleepMms = sleepMms + 1.0;
            }
        }
        else if (runningController == 1) {
            printf("[loadController]: controll time over \n");
            runningController = 0;
        }
    }
    
    sharedmem_detach(sharedcpuload);
    return EXIT_SUCCESS;

}