#include "lblib.hpp"


//calculat the cpu laod until a specified linux priority
//result is not super accurate - better results with average over 3 calls
//for better results call function readLoadIM(int minPriority)
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



//readLoadIM ... Imporved
//average cpuLoad over 5 calls of readLoad
float readLoadIM(int minPriority) {
    float cpuLoad = 0;
    for (int i = 0; i < 5; i++) {
        cpuLoad = cpuLoad + readLoad(minPriority);
        usleep(5);
    }
    return (cpuLoad / 5);
}

/*

//readLoadIM ... Imporved
//average cpuLoad over 10 calls of readLoad
float readLoadIM(int minPriority) {
    float cpuLoad = 0;
    for (int i = 0; i < 10; i++) {
        cpuLoad = cpuLoad + readLoad(minPriority);
    }
    return (cpuLoad / 10);
}

*/

/*
    average ten times over the result, decreases the peaks in the cpu load measure
    although 10 times is too long and the load balancer does not finish within the necessary time (only a problem when logging and comparing the cpu% of two different nodes)
    thus the function with reading 5 times is used, so that the load balancer does finish within the bounded time
*/


//function for calculating the NP of one specific task "taskNumber"
float readNP(int taskNumber) {
    float NP = -1.0;
    int K1;
    int K2; 
    int K3;
    std::__cxx11::string lineBuffer;
    lineBuffer.reserve(NP_FILE_LENGTH);
    std::ifstream myfile;
    myfile.open ("loadBalancingTask/NP.csv"); 
    if(myfile.is_open()) {
        for (int i = 0; i <= taskNumber; i++) {
            std::getline(myfile, lineBuffer);
        }
        K1 = lineBuffer[3] - 48;
        K2 = lineBuffer[5] - 48;
        K3 = lineBuffer[7] - 48;
        NP = ((float)K1 + (float)K2 + (float)K3) / 3;
    }
    else {
        std::cout << "[lblib readNP function]: NP.csv file could no be opened \n" << std::endl;
    }
    myfile.close();
    return NP;
	
}
    
    
    
//check if the task is executed on one node or on an other
//this check is performed by reading and compareing the mac address of eth0
bool amIInternally (void) {
    static const char internalMAC [] = "78:a5:04:fd:d2:12";
    char myMAC [18] = "\0";
    FILE *address = NULL;
    address = fopen("/sys/class/net/eth0/address", "r");
    fgets(myMAC, 18, address);
    fclose(address);
    for (int i = 0; i < 17; i++) {
        if(myMAC[i] != internalMAC[i]) {
            return 0;
        }
        else {
            continue;
        }
    }
    
    return 1;
}