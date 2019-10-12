#define _XOPEN_SOURCE 500

#include "ipclib.h"

#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>

#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

float readLoad(int minPriority) {
    
    if((minPriority > 39) || (minPriority < 0)) {
        printf("[readLoad function]: Error no valid minPriority\n");
        return -1.0; //check for an sensible input
    }
    
    float cpuLoadSum = 0.0;
    //write ps output to loadFile, relative path => same directory as the program
    system("ps -eo pid,uid,priority,ni,pcpu,comm --sort -pcpu > loadFile");
    
    //open file
    FILE *loadFile = NULL;
    loadFile = fopen("loadFile", "r");
    //variable decleration
    char lineBuffer [50] = "";
    int lineCounter = 0;
    int linePointerCPU = 0;
    int linePointerPRI = 0;
    char cPRI[] = "-139";
    char cCPU[] = "95.4";
    float fCPU = 0.0;
    int iPRI = 0;
    
    while(1) {
        //ToDo: variable lineBuffer length
        
        //if EOF break
        if(fgets(lineBuffer, 50, loadFile) == NULL) {
            break;
        }
        
        //read header line
        //get starting field of priority and %cpu
        if(lineCounter == 0) {
            lineCounter++;
            for (int i = 0; i < strlen(lineBuffer); i++) {
                if ((lineBuffer[i] == 'I') && (lineBuffer[i-1] == 'R') && (lineBuffer[i-2] == 'P') ) {
                    linePointerPRI = (i - 2);
                }
                if ((lineBuffer[i] == 'U') && (lineBuffer[i-1] == 'P') && (lineBuffer[i-2] == 'C') ) {
                    linePointerCPU = (i - 3);
                    break;
                }
            }
            continue;
        }
        //store pri and %cpu in arrays
        for(int i = 0; i < 4; i++) {
            cPRI[i] = lineBuffer[linePointerPRI + i];
            cCPU[i] = lineBuffer[linePointerCPU + i];            
        }
        
        //convert cCPU to float: all possible constelations of the string in the loadFile
        if((cCPU[2] == '.') && (cCPU[0] == ' ')) {
            fCPU = (((float)(cCPU[1])) - 48.0) + ((((float)(cCPU[3])) - 48.0) * 0.1);
        }
        else if ((cCPU[2] == '.') && (cCPU[0] != ' ')) {
            fCPU = ((((float)(cCPU[0])) - 48.0) * 10) + (((float)(cCPU[1])) - 48.0) + ((((float)(cCPU[3])) - 48.0) * 0.1);
        }
        else if((cCPU[2] != '.') && (cCPU[0] == ' ')){
            fCPU = ((((float)(cCPU[1])) - 48.0) * 100) + ((((float)(cCPU[2])) - 48.0) * 10) + (((float)(cCPU[3])) - 48.0);
        }
        else {
            printf("[readLoad function]: Error in loadFile %%CPU\n");
            fclose(loadFile);
            return -1.0; //if the string does not look like any of these formats
        }
        
        //convert cPRI to int: all possible constelations of the string in the loadFile
        if(((cPRI[0]) == '-') && ((cPRI[1]) == '1') && ((cPRI[2]) != ' ') && ((cPRI[3]) != ' ')) {
            iPRI = ((((cPRI[1] - 48) * 100) + ((cPRI[2] - 48) * 10) + (cPRI[3] - 48)) * -1);
        }
        else if(((cPRI[0]) == '-') && ((cPRI[1]) != ' ') && ((cPRI[2]) != ' ') && ((cPRI[3]) == ' ')) {
            iPRI = ((((cPRI[1] - 48) * 10) + ((cPRI[2] - 48))) * -1);
        }
        else if(((cPRI[0]) == ' ') && ((cPRI[1]) != ' ') && ((cPRI[2]) != ' ') && ((cPRI[3]) == ' ')) {
            iPRI = ((((cPRI[1] - 48) * 10) + ((cPRI[2] - 48))));
        }
        else if(((cPRI[0]) == '-') && ((cPRI[1]) != ' ') && ((cPRI[2]) = ' ') && ((cPRI[3]) == ' ')) {
            iPRI = (cPRI[1] - 48) * -1;
        }
        else if(((cPRI[0]) == ' ') && ((cPRI[1]) == ' ') && ((cPRI[2]) != ' ') && ((cPRI[3]) == ' ')) {
            iPRI = (cPRI[2] - 48);
        }
        else {
            printf("[readLoad function]: Error in loadFile %%PRI\n");
            fclose(loadFile);
            return -1.0; //if the string does not look like any of these formats
        }
        
        //sum %cpu
        if(iPRI <= minPriority) {
            cpuLoadSum = cpuLoadSum + fCPU; 
        }
        
        //loadFile sorted by %cpu - no need for any further entries
        if(fCPU == 0.0) {
            break;
        }
    }
    fclose(loadFile);
    return cpuLoadSum; 
}



//for testing

#define SHMSZ 4

int main(int argc, char *argv[]) {
    
    key_t semkey = 1234;
    int semid = semaphore_init(semkey);
    
    key_t shmkey = 5678;
    int shmid = sharedmem_init(shmkey, 4);
    
    float *cpuload = sharedmem_attach(shmid);
    
    while(1) {
        semaphore_operation(semid, LOCK);
        *cpuload = readLoad(20);
        sleep(3);
        printf("[readLoad]: CPU Load = %f \n", *cpuload);
        semaphore_operation(semid, UNLOCK);
        sleep(2);
    }
    

    return 0;
}