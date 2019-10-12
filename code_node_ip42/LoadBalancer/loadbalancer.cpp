//loadbalancer.cpp
//for compiling: 
//g++ loadbalancer.cpp /var/lib/cloud9/c_cpp_projects/ipclib/ipclib.c /var/lib/cloud9/c_cpp_projects/lblib/lblib.cpp -o loadbalancer

#include <iostream>
#include <signal.h>
#include <sys/time.h>
#include "/var/lib/cloud9/c_cpp_projects/ipclib/ipclib.h"
#include "/var/lib/cloud9/c_cpp_projects/lblib/lblib.hpp"
#include <vector>


//for colouring outputs in printf
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


//defines for shared memory size
#define SHM_SHAREDMEMORY_SIZE 13
#define SHM_LOADBALANCINGINTERFACE_SIZE 28
#define SHM_LOADBALANCING_ANSWER_INTERFACE_SIZE 28


//changable characteristic of load balancer
#define OUTSOUECING_BORDER 60.0
#define LB_CYCLE_TIME_MICROSEC 5000000


using namespace std;


int isRunning = 1;

//signal function
void controlCSig(int sig) {
    isRunning = 0;
} 

//returning the index of the array with the smallest number
int minIndex(float array[], int size) {
    int smallesIndex = 0;
    for (int i = 1; i < size; i++) {
        if(array[i] < array[smallesIndex]) {
            smallesIndex = i;
        }
    }
    return smallesIndex;
}

//for logging and evaluating the functionality
void writeCPULoadToCSV(float cpuload, unsigned int loopCounterVar, bool *outsourcingTasks) {
    FILE *filePointer = NULL;
    filePointer = fopen("cpuload.csv", "a");
    fprintf(filePointer, "%f; %u; %d; %d; %d; %d; %d; %d; %d;\n", cpuload, loopCounterVar, outsourcingTasks[0], outsourcingTasks[1], outsourcingTasks[2], outsourcingTasks[3], outsourcingTasks[4], outsourcingTasks[5], outsourcingTasks[6]);
    fclose(filePointer);
    filePointer = NULL;
}

int main(void) {
    
    //signal handlers
    signal(SIGINT, controlCSig);
    //ignore if a child process returns => automaticly reaped by init process
    signal(SIGCHLD, SIG_IGN);
    
    //
    struct timeval tv1, tv2;
    double time_elapsed;
    
    float CPULoad = 0.0;
    
    unsigned int loopCounterVar = 0;
    
    //initialise semaphore for shared memory
    key_t semKeySharedMemory = 1111;
    int semIDSharedMemory = semaphore_init(semKeySharedMemory);
    
    //initialise shared memory shared memory
    key_t shmKeySharedMemory = 1112;
    int shmIDSharedMemory = sharedmem_init(shmKeySharedMemory, SHM_SHAREDMEMORY_SIZE);
    bool *pSharedMemoryBase = (bool*) sharedmem_attach(shmIDSharedMemory);
    
    //initialise semaphore for load balancing task interface
    key_t semKeyLoadBalancingInterface = 3331;
    int semIDLoadBalancingInterface = semaphore_init(semKeyLoadBalancingInterface);
    
    //initialise shared memory load balancing task interface
    key_t shmKeyLoadBalancingInterface = 3332;
    int shmIDLoadBalancingInterface = sharedmem_init(shmKeyLoadBalancingInterface, SHM_LOADBALANCINGINTERFACE_SIZE);
    int *pLoadBalancingInterfaceBase = (int*) sharedmem_attach(shmIDLoadBalancingInterface);
    
    //initialise semaphore for load balancing answer interface
    key_t semKeyLoadBalancingAnswerInterface = 4441;
    int semIDLoadBalancingAnswerInterface = semaphore_init(semKeyLoadBalancingAnswerInterface);
    
    //initialise shared memory load balancing answer interface
    key_t shmKeyLoadBalancingAnserInterface = 4442;
    int shmIDLoadBalancingAnswerInterface = sharedmem_init(shmKeyLoadBalancingAnserInterface, SHM_LOADBALANCING_ANSWER_INTERFACE_SIZE);
    int *pLoadBalancingAnswerInterfaceBase = (int*) sharedmem_attach(shmIDLoadBalancingAnswerInterface);
    
    
    //pointers to elements of the shared memory representing the Load Balancing Tasks to invoke
    bool *pSharedMemoryTaskZero = pSharedMemoryBase + 0;
    bool *pSharedMemoryTaskOne = pSharedMemoryBase + 1;
    bool *pSharedMemoryTaskTwo = pSharedMemoryBase + 2;
    bool *pSharedMemoryTaskThree = pSharedMemoryBase + 3;
    bool *pSharedMemoryTaskFour = pSharedMemoryBase + 4;
    bool *pSharedMemoryTaskFive = pSharedMemoryBase + 5;
    bool *pSharedMemoryTaskSix = pSharedMemoryBase + 6;
    bool *pSharedMemoryInputVariable = pSharedMemoryBase + 7;
    bool *pSharedMemoryOutputVariable = pSharedMemoryBase + 8;
    bool *pSharedMemoryVariable = pSharedMemoryBase + 9;
    bool *pSharedMemoryGlobalInputVariable = pSharedMemoryBase + 10;
    bool *pSharedMemoryGlobalOutputVariable = pSharedMemoryBase +11;
    bool *pSharedMemoryGlobalVariable = pSharedMemoryBase + 12;
    
    
    //unsigned long long int is 64 bit in size - means 18,446,744,073,709,551,616 tasks before overflow ;)
    unsigned long long int tasks[7] = {0, 0, 0, 0, 0, 0, 0};
    
    //holds the node priorities for all tasks
    float taskNP [7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    //fill taskNP array
    for (int i = 0; i < 7; i++) {
        taskNP[i] = readNP(i); 
        printf("taskNP[i] = %f \n", taskNP[i]);
    }
    
    
    //the CPU load at which the outsourcing process starts
    float outsourceBorder = OUTSOUECING_BORDER;
    
    //hysteresis, the CPU load needs to be under this value for accepting tasks again
    float endOutsourcingHysteresis = outsourceBorder - 10.0;
    
    //absolute maximum of CPU load; anything beyond this point is a fatal error
    float maxTransinetOverload = outsourceBorder + 10.0;
    
    //marks the taks, that are currently outsourced
    bool outsourcingPerTask [7] = {0, 0, 0, 0, 0, 0, 0};
    
     //orderd tasks (index) by node priority
    int outsourceOrder [7] = {0, 0, 0, 0, 0, 0, 0};
    
    float *taskNP2 = new float [7];
    for (int i = 0; i < 7; i++) {
        taskNP2[i] = taskNP[i];
    }
    
    //order tasks by node priority and store in outsourceOrder array
    for (int i = 0; i < 7; i++) {
        outsourceOrder[i] = minIndex(taskNP2, 7);
        taskNP2[outsourceOrder[i]] = 10.0;
        printf("outsourceOrder[i] = %i \n", outsourceOrder[i]);
    }
    
    delete [] taskNP2;
    
    //holds dynamicly created internal Tasks
    vector <LoadBalancingTask> externalTasks(0);
    
    //holds dynamicly created external Tasks
    vector <LoadBalancingTask> internalTasks(0);
    
    //holds the pahts to all load balancing tasks
    vector <string> path {"/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/databaseAccess", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/websiteCall", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/imageProcessing", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/averageCalc", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/sendEmail", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/userInput", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/blinky"};

    while(isRunning) {
        
        //start timer
        gettimeofday(&tv1, NULL);
        
        //read the current cpu load
        //two diffrent versions of readLoadIM(): 
        //  => readLoad 10 times and average => smaller spikes, better to controll - long runtime
        //  => readLoad 5 times and  average => shorter runtime - larger spikes - not so good to controll
        // look at lblib.cpp for insight
        CPULoad = readLoadIM(20);
        
        
        //colour output for better visability
        if(CPULoad < outsourceBorder) {
            printf(ANSI_COLOR_GREEN "CPULoad = %f \n" ANSI_COLOR_RESET, CPULoad);
        }
        else if ((CPULoad >= outsourceBorder) && (CPULoad < maxTransinetOverload)){
            printf(ANSI_COLOR_YELLOW "CPULoad = %f \n" ANSI_COLOR_RESET, CPULoad);
        }
        else {
            printf(ANSI_COLOR_RED "CPULoad = %f \n" ANSI_COLOR_RESET, CPULoad);
        }
        
        
        
        //fill outsourcingPerTask array 
        //every field corresponds to a task which is either 1 - outsourced or 0 - internally
        if(CPULoad > outsourceBorder) {
            for (int i = 0; i < 7; i++) {
                if((taskNP[i] <= (CPULoad - outsourceBorder))) {
                    outsourcingPerTask[i] = 1;
                }
            }
        }
        
        //reset the outsourcing when the hysteresi's lower border is reached
        if (CPULoad < endOutsourcingHysteresis) {
            for (int i = 0; i < 7; i++) {
                outsourcingPerTask[i] = 0;
            }    
        }
        
        //for writing the csv and interpreting the result
        loopCounterVar = loopCounterVar + 1;
        writeCPULoadToCSV(CPULoad, loopCounterVar, outsourcingPerTask);
        printf("loopCounterVar = %u \n", loopCounterVar);
        
        //basically for debugging
        printf("outsourcingPerTask = {%d, %d, %d, %d, %d, %d, %d}\n", outsourcingPerTask[0], outsourcingPerTask[1], outsourcingPerTask[2], outsourcingPerTask[3], outsourcingPerTask[4], outsourcingPerTask[5], outsourcingPerTask[6]);
        
        //cycel through all tasks and call them externally or internally
        for (int i = 0; i < 7; i++) {
            
            semaphore_operation(semIDSharedMemory, LOCK);
            
            //look at all called tasks in the shared memory
            if(pSharedMemoryBase[i] == 1) {
                tasks[i] = tasks[i] + 1; 
                
                //check if the task should be outsourced or not 
                if(outsourcingPerTask[i] == 0) {
                    char *cpath = new char[path.at(i).length() + 1];
                    strcpy (cpath, path.at(i).c_str());
                    //call the task
                    int taskPID = callTask(cpath);
                    //create the object
                    //still using old library lblib
                    internalTasks.push_back(LoadBalancingTask(i, 1, taskPID));
                    delete [] cpath;
                    usleep(10);
                }
                
                //if the specific task should be outsourced
                if (outsourcingPerTask[i] == 1) {
                    semaphore_operation(semIDLoadBalancingInterface, LOCK);
                    //write load balancing interface
                    pLoadBalancingInterfaceBase[i] = pLoadBalancingInterfaceBase[i] + 1;
                    semaphore_operation(semIDLoadBalancingInterface, UNLOCK);
                    
                    //create object
                    externalTasks.push_back(LoadBalancingTask(i, 0, -1));
                }
                
                pSharedMemoryBase[i] = 0;
            }
            semaphore_operation(semIDSharedMemory, UNLOCK);
        }
        
        //for debugging, but also good to know at runtime
        printf("tasks = {%llu, %llu, %llu, %llu, %llu, %llu, %llu} \n", tasks[0], tasks[1], tasks[2], tasks[3], tasks[4], tasks[5], tasks[6]);
        printf("internalTasks.size() = %i \n", internalTasks.size());
        printf("externalTasks.size() = %i \n", externalTasks.size());
        
        //delete object internally
        //runtime ~ O(N) is there any better algorithem? 
        for(int i = 0; i < internalTasks.size(); i++) {
            if(!checkPID(internalTasks.at(i).getPID())) {
                continue;
            }
            else {
                tasks[internalTasks.at(i).getTaskNumber()] = tasks[internalTasks.at(i).getTaskNumber()] - 1;
                internalTasks.erase(internalTasks.begin()+i);
            }
        }
        
        //delete objects externally
        //worst case runtime O(N^2)?? better algorithem needed
        semaphore_operation(semIDLoadBalancingAnswerInterface, LOCK);
        for(int i = 0; i < 7; i++) {
            if(pLoadBalancingAnswerInterfaceBase[i] > 0) {
                for(int j = 0; j < externalTasks.size(); j++) {
                    if(externalTasks.at(j).getTaskNumber() == i) {
                        tasks[i] = tasks[i] - 1;
                        externalTasks.erase(externalTasks.begin()+j);
                        pLoadBalancingAnswerInterfaceBase[i] = pLoadBalancingAnswerInterfaceBase[i] - 1;
                        break;
                    }
                    else {
                        continue;
                    }
                }
            }
        }
        semaphore_operation(semIDLoadBalancingAnswerInterface, UNLOCK);
        
        //stop timer
        gettimeofday(&tv2, NULL);
        
        //calculate timer difference
        //only needed for interpreting the result with cpuload.csv
        time_elapsed = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
        int sleepTime = LB_CYCLE_TIME_MICROSEC - (time_elapsed * 1000000);
        if(sleepTime <= 0) {
            printf(ANSI_COLOR_RED "[loadbalancer]: LB_CYCLE_TIME_MICROSEC too low \n" ANSI_COLOR_RESET);
        }
        else {
            usleep(sleepTime);    
        }
        
    }
    
    //detach shared memory and load balancing interface
    sharedmem_detach(pSharedMemoryBase);
    
    //detach load balancing interface
    sharedmem_detach(pLoadBalancingInterfaceBase);
    sharedmem_detach(pLoadBalancingAnswerInterfaceBase);
    
    //destroy load balancing interface
    sharedmem_destroy(shmIDLoadBalancingInterface);
    sharedmem_destroy(shmIDLoadBalancingAnswerInterface);
    
    //destroy semaphore
    semaphore_destroy(semIDLoadBalancingInterface);
    semaphore_destroy(semIDLoadBalancingAnswerInterface);
    
    return 0;
}