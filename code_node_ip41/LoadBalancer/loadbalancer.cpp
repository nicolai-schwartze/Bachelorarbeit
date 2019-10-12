//loadbalancer.cpp
//for compiling: 
//g++ loadbalancer.cpp /var/lib/cloud9/c_cpp_projects/ipclib/ipclib.c /var/lib/cloud9/c_cpp_projects/lblib/lblib.cpp -o loadbalancer


#include <iostream>
#include <signal.h>
#include <sys/time.h>
#include "/var/lib/cloud9/c_cpp_projects/ipclib/ipclib.h"
//still old version => should be updated to lblib_v2.hpp
#include "/var/lib/cloud9/c_cpp_projects/lblib/lblib.hpp"
#include <vector>

//colouring output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define SHM_LOADBALANCINGINTERFACE_SIZE 28
#define SHM_LOADBALANCING_ANSWER_INTERFACE_SIZE 28

//cycle time for evaluating the result and debugging
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

//evaluating the result
void writeCPULoadToCSV(float cpuload, unsigned int countVar, unsigned long long int *tasksS) {
    FILE *filePointer = NULL; 
    filePointer = fopen("cpuload.csv", "a");
    fprintf(filePointer, "%f; %u; %i; %i; %i; %i; %i; %i; %i;\n", cpuload, countVar, tasksS[0], tasksS[1], tasksS[2], tasksS[3], tasksS[4], tasksS[5], tasksS[6]);
    fclose(filePointer);
    filePointer = NULL;
}
    

int main(void) {
    
    //signal handlers
    signal(SIGINT, controlCSig);
    signal(SIGCHLD, SIG_IGN);
    
    struct timeval tv1, tv2;
    double time_elapsed;
    
    //initialise semaphore for load balancing interface
    key_t semKeyLoadBalancingInterface = 3331;
    int semIDLoadBalancingInterface = semaphore_init(semKeyLoadBalancingInterface);
    
    //initialise shared memory load balancing task interface
    key_t shmKeyLoadBalancingInterface = 3332;
    int shmIDLoadBalancingInterface = sharedmem_init(shmKeyLoadBalancingInterface, SHM_LOADBALANCINGINTERFACE_SIZE);
    int *pLoadBalancingInterfaceBase = (int*) sharedmem_attach(shmIDLoadBalancingInterface);
    
    key_t semKeyLoadBalancingAnswerInterface = 4441;
    int semIDLoadBalancingAnswerInterface = semaphore_init(semKeyLoadBalancingAnswerInterface);
    
    //initialise shared memory load balancing task interface
    key_t shmKeyLoadBalancingAnswerInterface = 4442;
    int shmIDLoadBalancingAnswerInterface = sharedmem_init(shmKeyLoadBalancingAnswerInterface, SHM_LOADBALANCING_ANSWER_INTERFACE_SIZE);
    int *pLoadBalancingAnswerInterfaceBase = (int*) sharedmem_attach(shmIDLoadBalancingAnswerInterface);
    
    //set tasks in load balancing answer interface to zero
    for (int i = 0; i < 7; i++) {
        pLoadBalancingAnswerInterfaceBase[i] = 0;
    }
    
    
    //unsigned long long int is 64 bit in size - means 18,446,744,073,709,551,616 tasks before overflow ;)
    unsigned long long int tasks[7] = {0, 0, 0, 0, 0, 0, 0};
    
    //holds dynamicly created internal Tasks
    vector <LoadBalancingTask> internalTasks(0);
    
    //holds the pahts to all load balancing tasks
    vector <string> path {"/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/databaseAccess", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/websiteCall", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/imageProcessing", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/averageCalc", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/sendEmail", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/userInput", 
                          "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/blinky"};

    unsigned int loopCounterVar = 0;
    float CPULoad = 0.0;
    
    
    while(isRunning) {
        
        //start timer
        gettimeofday(&tv1, NULL);
        
        //read cpu load 
        //function of two different flavours => for insight look at lblib.cpp
        CPULoad = readLoadIM(20);
        loopCounterVar = loopCounterVar + 1;
        writeCPULoadToCSV(CPULoad, loopCounterVar, tasks);
        printf("loopCounterVar = %u \n", loopCounterVar);
        
        //cycel through all tasks and call them externally or internally
        for (int i = 0; i < 7; i++) {
            
            semaphore_operation(semIDLoadBalancingInterface, LOCK);
            //cycle through tasks the load balancer on node 42 has outsourced
            //on a real system, the plc could also write to this interface
            if(pLoadBalancingInterfaceBase[i] != 0) {
                tasks[i] = tasks[i] + pLoadBalancingInterfaceBase[i]; 
                
                char *cpath = new char[path.at(i).length() + 1];
                strcpy (cpath, path.at(i).c_str());
                for(int j = 0; j < pLoadBalancingInterfaceBase[i]; j++) {
                    //call the task
                    int taskPID = callTask(cpath);
                    //create the object
                    internalTasks.push_back(LoadBalancingTask(i, 1, taskPID));
                }
                //deallocate path string
                delete [] cpath;
                
                pLoadBalancingInterfaceBase[i] = 0;
            }
            semaphore_operation(semIDLoadBalancingInterface, UNLOCK);
        }
        
        //for debugging and evaluating the result
        printf("tasks = {%llu, %llu, %llu, %llu, %llu, %llu, %llu} \n", tasks[0], tasks[1], tasks[2], tasks[3], tasks[4], tasks[5], tasks[6]);
        printf("internalTasks.size() = %i \n", internalTasks.size());
        
        
        //delete object internally
        //runtime ~ O(N) is there any better algorithem? 
        for(int i = 0; i < internalTasks.size(); i++) {
            if(!checkPID(internalTasks.at(i).getPID())) {
                continue;
            }
            else {
                //delete task and write via the load balancing answer interface to client
                semaphore_operation(semIDLoadBalancingAnswerInterface, LOCK);
                tasks[internalTasks.at(i).getTaskNumber()] = tasks[internalTasks.at(i).getTaskNumber()] - 1;
                pLoadBalancingAnswerInterfaceBase[internalTasks.at(i).getTaskNumber()] = pLoadBalancingAnswerInterfaceBase[internalTasks.at(i).getTaskNumber()] + 1;
                internalTasks.erase(internalTasks.begin()+i);
                semaphore_operation(semIDLoadBalancingAnswerInterface, UNLOCK);
            }
        }
        
        //stop timer and sleep for the remaining time
        gettimeofday(&tv2, NULL);
        time_elapsed = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
        int sleepTime = LB_CYCLE_TIME_MICROSEC - (time_elapsed * 1000000);
        if(sleepTime <= 0) {
            printf(ANSI_COLOR_RED "[loadbalancer]: LB_CYCLE_TIME_MICROSEC too low \n" ANSI_COLOR_RESET);
        }
        else {
            usleep(sleepTime);    
        }
        
    }
    
    //detach load balancing interface
    sharedmem_detach(pLoadBalancingInterfaceBase);
    
    //destroy load balancing interface
    sharedmem_destroy(shmIDLoadBalancingInterface);
    
    //destroy semaphore
    semaphore_destroy(semIDLoadBalancingInterface);
    
    return 0;
}