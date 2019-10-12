//plc.c
//use for compiling:
//gcc -std=c99 -D_XOPEN_SOURCE=500 plc.c /var/lib/cloud9/c_cpp_projects/ipclib/ipclib.c /var/lib/cloud9/c_cpp_projects/gpiolib/gpiolib.c -o plc

#include "/var/lib/cloud9/c_cpp_projects/ipclib/ipclib.h"
#include "/var/lib/cloud9/c_cpp_projects/gpiolib/gpiolib.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define SHM_SHAREDMEMORY_SIZE 13
#define SHM_GLOBALVARABLEINTERFACE_SIZE 6
#define INPUT_PIN 47
#define OUTPUT_PIN 45

//plc metric - change characteristic of the load generation
#define PLC_CYCLE_TIME_MICROSEC 5000000
#define MAX_LOAD_AFTER_CYCLES 50



//global variables
_Bool isRunning = 1;

//signal function for handling ctrl+c
//may not contain non reentrend functions such as printf
void controlCSig(int sig) {
    isRunning = 0;
}

//simple function for creating an increasing load
int loadFunction(int value) {
    if(value >= MAX_LOAD_AFTER_CYCLES) {
        return 1;
    }
    else {
        float something = ((10.0 - (9.0/((float)(MAX_LOAD_AFTER_CYCLES)))*((float)value))); 
        return something;
    }
}

int main (int argc, char *argv[]) {
    
    //set variables for cycle time controlling
    struct timeval tv1, tv2;
    double time_elapsed;
    
    //introduce signal for termination
    signal(SIGINT, controlCSig);
    
    //input pin setup
    openGPIO(47, 'i');
    _Bool inputVal = digitalRead(47);
    
    //output pin setup
    openGPIO(45, 'o');
    _Bool outputVal = 0;
    digitalWrite(45, outputVal);
    
    //global variable
    _Bool globalVariableTest = 0;
    
    //user input variables on forking asynchronic load balancing tasks
    //check for valid input
    if (argc != 8) {
        printf("[PLC]: wrong number of input arguments \n");
        printf("[PLC]: input 7 numbers (0 or 1) for 7 Load Balancing Tasks \n");
        printf("[PLC]: sample input ./plc 0 1 0 1 1 0 0 \n");
        return 0;
    }
    _Bool checkForArgument = 0;
    for (int i = 1; i <= 7; i++) {
        if ((*argv[i] != '1') && (*argv[i] != '0')) {
            printf(ANSI_COLOR_RED "[PLC]: wrong argument at place %i \n" ANSI_COLOR_RESET, i);
            checkForArgument = 1;
        }
    }
    if (checkForArgument == 1) {
        return 0;
    }
    //set userInput Variable accordingly
    _Bool userInputTaskZero = 0;
    _Bool userInputTaskOne = 0;
    _Bool userInputTaskTwo = 0;
    _Bool userInputTaskThree = 0;
    _Bool userInputTaskFour = 0;
    _Bool userInputTaskFive = 0;
    _Bool userInputTaskSix = 0;
    
    if(*argv[1] == '1') {
        userInputTaskZero = 1;
    }
    if(*argv[2] == '1') {
        userInputTaskOne = 1;
    }
    if(*argv[3] == '1') {
        userInputTaskTwo = 1;
    }
    if(*argv[4] == '1') {
        userInputTaskThree = 1;
    }
    if(*argv[5] == '1') {
        userInputTaskFour = 1;
    }
    if(*argv[6] == '1') {
        userInputTaskFive = 1;
    }
    if(*argv[7] == '1') {
        userInputTaskSix = 1;
    }
    
    //initialise pseudo random number generator
    srand(time(NULL));
    
    //initialise semaphore for shared memory
    key_t semKeySharedMemory = 1111;
    int semIDSharedMemory = semaphore_init(semKeySharedMemory);
    
    //initialise shared memory shared memory
    key_t shmKeySharedMemory = 1112;
    int shmIDSharedMemory = sharedmem_init(shmKeySharedMemory, SHM_SHAREDMEMORY_SIZE);
    _Bool *pSharedMemoryBase = sharedmem_attach(shmIDSharedMemory);
    
    //pointers to elements of the shared memory representing the Load Balancing Tasks to invoke
    _Bool *ptaskZero = pSharedMemoryBase + 0;
    _Bool *ptaskOne = pSharedMemoryBase + 1;
    _Bool *ptaskTwo = pSharedMemoryBase + 2;
    _Bool *ptaskThree = pSharedMemoryBase + 3;
    _Bool *ptaskFour = pSharedMemoryBase + 4;
    _Bool *ptaskFive = pSharedMemoryBase + 5;
    _Bool *ptaskSix = pSharedMemoryBase + 6;
    _Bool *pSharedMemoryInputVariable = pSharedMemoryBase + 7;
    _Bool *pSharedMemoryOutputVariable = pSharedMemoryBase + 8;
    _Bool *pSharedMemoryVariable = pSharedMemoryBase + 9;
    _Bool *pSharedMemoryGlobalInputVariable = pSharedMemoryBase + 10;
    _Bool *pSharedMemoryGlobalOutputVariable = pSharedMemoryBase +11;
    _Bool *pSharedMemoryGlobalVariable = pSharedMemoryBase + 12; 
    
    //initialise values
    semaphore_operation(semIDSharedMemory, -1);
    *ptaskZero = 0;
    *ptaskOne = 0;
    *ptaskTwo = 0;
    *ptaskThree = 0;
    *ptaskFour = 0;
    *ptaskFive = 0;
    *ptaskSix = 0;
    *pSharedMemoryInputVariable = 0;
    *pSharedMemoryOutputVariable = 0;
    *pSharedMemoryVariable = 0;
    *pSharedMemoryGlobalInputVariable = 0;
    *pSharedMemoryGlobalOutputVariable = 0;
    *pSharedMemoryGlobalVariable = 0; 
    semaphore_operation(semIDSharedMemory, 1);
    
    //initialise semaphore globalVariableInterface
    key_t semKeyGlobalVariableInterface = 2221;
    int semIDGlobalVariableInterface = semaphore_init(semKeyGlobalVariableInterface);
    
    //initialise shared memory globalVariableInterface
    key_t shmKeyGlobalVariableInterface = 2222;
    int shmIDGlobalVariableInterface = sharedmem_init(shmKeyGlobalVariableInterface, SHM_GLOBALVARABLEINTERFACE_SIZE); 
    _Bool *pGlobalVariableInterfaceBase = sharedmem_attach(shmIDGlobalVariableInterface);
    
    //pointers to elements of the Global Variable Interface representing Input, Output and Variable
    semaphore_operation(semIDGlobalVariableInterface, -1);
    _Bool *pGlobalVariableInterfaceInputSetter = pGlobalVariableInterfaceBase + 0;
    _Bool *pGlobalVariableInterfaceOutputSetter = pGlobalVariableInterfaceBase + 1;
    _Bool *pGlobalVariableInterfaceVariableSetter = pGlobalVariableInterfaceBase + 2;
    _Bool *pGlobalVariableInterfaceInputGetter = pGlobalVariableInterfaceBase + 3;
    _Bool *pGlobalVariableInterfaceOutputGetter = pGlobalVariableInterfaceBase + 4;
    _Bool *pGlobalVariableInterfaceVariableGetter = pGlobalVariableInterfaceBase + 5;
    
    //initialise to erase old and not valid values
    *pGlobalVariableInterfaceInputSetter = 0;
    *pGlobalVariableInterfaceOutputSetter = 0;
    *pGlobalVariableInterfaceVariableSetter = 0;
    *pGlobalVariableInterfaceInputGetter = 0;
    *pGlobalVariableInterfaceOutputGetter = 0;
    *pGlobalVariableInterfaceVariableGetter = 0;
    
    semaphore_operation(semIDGlobalVariableInterface, 1);
    
    //for randomly generating load balancing tasks
    int loopCounterVar = 0;
    
    while (isRunning) {
        
        //start timer
        gettimeofday(&tv1, NULL);
        
        //get digital inputs
        inputVal = digitalRead(INPUT_PIN);
        
        //copy the global variable into the shared memory
        //normally done by the framework/runtime
        //only the framework has access to both, the shared memory and the global variable interface
        semaphore_operation(semIDGlobalVariableInterface, LOCK);
        semaphore_operation(semIDSharedMemory, LOCK);
        *pSharedMemoryGlobalInputVariable = *pGlobalVariableInterfaceInputSetter;
        *pSharedMemoryGlobalOutputVariable = *pGlobalVariableInterfaceOutputSetter;
        *pSharedMemoryGlobalVariable = *pGlobalVariableInterfaceVariableSetter;
        semaphore_operation(semIDSharedMemory, UNLOCK);
        semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
        
        //fork asynchronic programs
        //can be done by the user
        printf("[PLC]: loopCounterVar = %i \n", loopCounterVar);
        semaphore_operation(semIDSharedMemory, LOCK);
        if ((loopCounterVar % loadFunction(loopCounterVar)) == 0) {
            printf(ANSI_COLOR_YELLOW "[PLC]: forked \n" ANSI_COLOR_RESET);
            //if the user input allows that task
            if(userInputTaskZero == 1) {
                //if the load balancer started the task
                if(*ptaskZero == 0) {
                    *ptaskZero = 1;
                }
            }
            if(userInputTaskOne == 1) {
                if(*ptaskOne == 0) {
                    *ptaskOne = 1;
                }
            }
            if(userInputTaskTwo == 1) {
                if(*ptaskTwo == 0) {
                    *ptaskTwo = 1;
                }
            }
            if(userInputTaskThree == 1) {
                if(*ptaskThree == 0) {
                    *ptaskThree = 1;
                }
            }
            if(userInputTaskFour == 1) {
                if(*ptaskFour == 0) {
                    *ptaskFour = 1;
                }
            }
            if(userInputTaskFive == 1) {
                if(*ptaskFive == 0) {
                    *ptaskFive = 1;
                }
            }
        }
        
        //task 6 (7) this is the task blinky
        //only call once => multiple operation on pin output is not sensible
        if(loopCounterVar == 0) {
            if(userInputTaskSix == 1) {
                if(*ptaskSix == 0) {
                    *ptaskSix = 1;
                }
            }
        }
        
        
            
        semaphore_operation(semIDSharedMemory, UNLOCK);
        loopCounterVar = loopCounterVar +1;
        
        
        
        //actual program code, alterable by the user
        if (globalVariableTest == 0) {
            globalVariableTest = 1;   
        }
        
        semaphore_operation(semIDSharedMemory, LOCK);
        semaphore_operation(semIDGlobalVariableInterface, LOCK);
        //left from debugging
        //printf("*pSharedMemoryOutputVariable       %d \n", *pSharedMemoryOutputVariable);
        //printf("*pSharedMemoryGlobalOutputVariable %d \n", *pSharedMemoryGlobalOutputVariable);
        if((*pSharedMemoryOutputVariable) || (*pSharedMemoryGlobalOutputVariable)) {
            outputVal = 1;
        }
        else {
            outputVal = 0;
        }
        semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
        semaphore_operation(semIDSharedMemory, UNLOCK);
        /*  Actual
            PLC 
            Programm
            defined
            by
            the
            user
        */
        printf("[PLC]: outputVal = %d \n", outputVal);
        
        //write calculated outputs
        digitalWrite(OUTPUT_PIN, outputVal);
        
        //copy the shared memory back into the global variable interface
        //normally done by the framework/runtime environment
        semaphore_operation(semIDGlobalVariableInterface, LOCK);
        semaphore_operation(semIDSharedMemory, LOCK);
        //*pSharedMemoryOutputVariable = outputVal;
        *pSharedMemoryInputVariable = inputVal;
        *pSharedMemoryVariable = globalVariableTest;
        *pSharedMemoryGlobalInputVariable = *pSharedMemoryInputVariable;
        *pSharedMemoryGlobalOutputVariable = *pSharedMemoryOutputVariable;
        *pSharedMemoryGlobalVariable = *pSharedMemoryGlobalVariable;
        *pGlobalVariableInterfaceInputGetter = *pSharedMemoryGlobalInputVariable;
        *pGlobalVariableInterfaceOutputGetter = *pSharedMemoryGlobalOutputVariable;
        *pGlobalVariableInterfaceVariableGetter = *pSharedMemoryGlobalVariable;
        semaphore_operation(semIDSharedMemory, UNLOCK);
        semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
        
        //stop timer
        gettimeofday(&tv2, NULL);
        
        //calculate timer difference
        time_elapsed = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
        int sleepTime = PLC_CYCLE_TIME_MICROSEC - (time_elapsed * 1000000);
        if(sleepTime <= 0) {
            printf(ANSI_COLOR_RED "[PLC]: PLC_CYCLE_TIME_MICROSEC too low \n" ANSI_COLOR_RESET);
            isRunning = 0;
        }
        else {
            usleep(sleepTime);    
        }
        
    }
    
    //close GPIOs
    closeGPIO (47);
    closeGPIO (45);
    
    //destroy semaphores
    semaphore_destroy(semIDSharedMemory);
    semaphore_destroy(semIDGlobalVariableInterface);
    
    //detach shared memory and global variable interface
    sharedmem_detach(pSharedMemoryBase);
    sharedmem_detach(pGlobalVariableInterfaceBase);
    
    //destroy shared memory and global variable interface
    sharedmem_destroy(shmIDSharedMemory);
    sharedmem_destroy(shmIDGlobalVariableInterface);
    
    printf(ANSI_COLOR_RED "[PLC]: terminate PLC \n" ANSI_COLOR_RESET);
    
    return 0;
}