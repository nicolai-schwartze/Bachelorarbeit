//opc_ua_client.c
//for compiling: 
//gcc -std=c99 -D_XOPEN_SOURCE=500 opc_ua_client.c /var/lib/cloud9/c_cpp_projects/opclib/open62541.c /var/lib/cloud9/c_cpp_projects/ipclib/ipclib.c -o opc_ua_client

#include <stdio.h>
#include "/var/lib/cloud9/c_cpp_projects/opclib/open62541.h"
#include <unistd.h>
#include "/var/lib/cloud9/c_cpp_projects/ipclib/ipclib.h"

//colours in standard output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//defines for shared memory size
#define SHM_LOADBALANCING_ANSWER_INTERFACE_SIZE 28
#define SHM_GLOBALVARABLEINTERFACE_SIZE 6

_Bool isRunning = 1;

//signal function for interrupting the cycle
void controlCSig(int sig) {
    isRunning = 0;
} 

int main(void) {
    
    //signal handler
    signal(SIGINT, controlCSig);
    
    //initialise semaphore for load balancing interface
    key_t semKeyLoadBalancingAnswerInterface = 4441;
    int semIDLoadBalancingAnswerInterface = semaphore_init(semKeyLoadBalancingAnswerInterface);
    
    //initialise shared memory load balancing task interface
    key_t shmKeyLoadBalancingAnswerInterface = 4442;
    int shmIDLoadBalancingAnswerInterface = sharedmem_init(shmKeyLoadBalancingAnswerInterface, SHM_LOADBALANCING_ANSWER_INTERFACE_SIZE);
    int *pLoadBalancingAnswerInterfaceBase = (int*) sharedmem_attach(shmIDLoadBalancingAnswerInterface);
    
    key_t semKeyGlobalVariableInterface = 2221;
    int semIDGlobalVariableInterface = semaphore_init(semKeyGlobalVariableInterface);
    
    //initialise shared memory globalVariableInterface
    key_t shmKeyGlobalVariableInterface = 2222;
    int shmIDGlobalVariableInterface = sharedmem_init(shmKeyGlobalVariableInterface, SHM_GLOBALVARABLEINTERFACE_SIZE); 
    _Bool *pGlobalVariableInterfaceBase = sharedmem_attach(shmIDGlobalVariableInterface);
    
    semaphore_operation(semIDGlobalVariableInterface, LOCK);
    _Bool *pGlobalVariableInterfaceInputSetter = pGlobalVariableInterfaceBase + 0;
    _Bool *pGlobalVariableInterfaceOutputSetter = pGlobalVariableInterfaceBase + 1;
    _Bool *pGlobalVariableInterfaceVariableSetter = pGlobalVariableInterfaceBase + 2;
    _Bool *pGlobalVariableInterfaceInputGetter = pGlobalVariableInterfaceBase + 3;
    _Bool *pGlobalVariableInterfaceOutputGetter = pGlobalVariableInterfaceBase + 4;
    _Bool *pGlobalVariableInterfaceVariableGetter = pGlobalVariableInterfaceBase + 5;
    
    *pGlobalVariableInterfaceInputSetter = 0;
    *pGlobalVariableInterfaceOutputSetter = 0;
    *pGlobalVariableInterfaceVariableSetter = 0;
    *pGlobalVariableInterfaceInputGetter = 0;
    *pGlobalVariableInterfaceOutputGetter = 0;
    *pGlobalVariableInterfaceVariableGetter = 0;
    semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
    
    //character string for nodeID
    char name[] = "answer Server X";
    
    //client configurations
    UA_ClientConfig conf = UA_ClientConfig_default;
    conf.secureChannelLifeTime = 60*60*1000;
    UA_Client *client = UA_Client_new(conf);
    
    //connect to ther server
    while (1) {
    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://10.0.0.42:4840");
        if(retval != UA_STATUSCODE_GOOD) {
            printf(ANSI_COLOR_RED "[opc_ua_client]: server 10.0.0.42:4840 not responding \n" ANSI_COLOR_RESET);
            return (int)retval;
        }
        else {
            break;
        }
    }
    printf(ANSI_COLOR_GREEN "[opc_ua_client]: connected to server 10.0.0.42:4840 \n" ANSI_COLOR_RESET);
    

    UA_Variant value; 
    UA_Variant_init(&value);

    //continuous cycle
    while(isRunning) {
        semaphore_operation(semIDLoadBalancingAnswerInterface, LOCK);
        //read the load balancing answer interface form the load balancer and write the answers to the server
        for (int i = 0; i < 7; i++) {
            if (pLoadBalancingAnswerInterfaceBase[i] != 0) {
                UA_Variant value; 
                UA_Variant_init(&value);
                UA_Variant value2;
                UA_Variant_init(&value2);
                name[14] = (char)(i + 48);
                const UA_NodeId nodeId = UA_NODEID_STRING(1, name);
                UA_Client_readValueAttribute(client, nodeId, &value2);
                UA_Int32 intermediateTaksInteger = pLoadBalancingAnswerInterfaceBase[i] + *(UA_Int32 *)value2.data
                UA_Variant_setScalar(&value, &intermediateTaksInteger, &UA_TYPES[UA_TYPES_INT32]);
                UA_Client_writeValueAttribute(client, nodeId, &value);
                pLoadBalancingAnswerInterfaceBase[i] = 0;
            }
            else {
                continue;
            }
        }
        semaphore_operation(semIDLoadBalancingAnswerInterface, UNLOCK);
        
        semaphore_operation(semIDGlobalVariableInterface, LOCK);
        //if the clobal variable output setter is true, write true to server
        if(*pGlobalVariableInterfaceOutputSetter != 0) {
            UA_Variant value; 
            UA_Variant_init(&value);
            UA_Boolean intermediateTaksBoolean = 1;
            UA_Variant_setScalar(&value, &intermediateTaksBoolean, &UA_TYPES[UA_TYPES_BOOLEAN]);
            const UA_NodeId nodeIdone = UA_NODEID_STRING(1, "globalVarOutputSetter");
            UA_Client_writeValueAttribute(client, nodeIdone, &value);
        }
        else {
            //else write 0 to server
            UA_Variant value; 
            UA_Variant_init(&value);
            UA_Boolean intermediateTaksBoolean = 0;
            UA_Variant_setScalar(&value, &intermediateTaksBoolean, &UA_TYPES[UA_TYPES_BOOLEAN]);
            const UA_NodeId nodeIdzero = UA_NODEID_STRING(1, "globalVarOutputSetter");
            UA_Client_writeValueAttribute(client, nodeIdzero, &value);
        }
        semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
        
        
        semaphore_operation(semIDGlobalVariableInterface, LOCK);
        //if the global variable setter is true, write true to server
        if(*pGlobalVariableInterfaceVariableSetter != 0) {
            UA_Variant value; 
            UA_Variant_init(&value);
            UA_Boolean intermediateTaksBoolean = 1;
            UA_Variant_setScalar(&value, &intermediateTaksBoolean, &UA_TYPES[UA_TYPES_BOOLEAN]);
            const UA_NodeId nodeId = UA_NODEID_STRING(1, "globalVarVariableSetter");
            UA_Client_writeValueAttribute(client, nodeId, &value);   
        }
        else {
            //else write zero to server
            UA_Variant value; 
            UA_Variant_init(&value);
            UA_Boolean intermediateTaksBoolean = 0;
            UA_Variant_setScalar(&value, &intermediateTaksBoolean, &UA_TYPES[UA_TYPES_BOOLEAN]);
            const UA_NodeId nodeId = UA_NODEID_STRING(1, "globalVarVariableSetter");
            UA_Client_writeValueAttribute(client, nodeId, &value); 
        }
        semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
        
        //check if still connected to the server => if not, disconnect
        UA_StatusCode testConnection = UA_Client_connect(client, "opc.tcp://10.0.0.42:4840");
        if(testConnection != UA_STATUSCODE_GOOD) {
            printf(ANSI_COLOR_RED "[opc_ua_client]: server 10.0.0.42:4840 connection lost \n" ANSI_COLOR_RESET);
            isRunning = 0;
        }
        
        //read heartbeat from server to keep session alive
        if(isRunning) {
            UA_Variant value;
            UA_Variant_init(&value);
            UA_StatusCode retvalTemp = UA_Client_connect(client, "opc.tcp://localhost:4840");
            const UA_NodeId heartBeatNID = UA_NODEID_STRING(1, "heartBeat");
            retvalTemp = UA_Client_readValueAttribute(client, heartBeatNID, &value);
        }
        
        usleep(50000);
    }
    
    // Clean up
    //disconnect client
    UA_Client_delete(client); 
    
    //clear shared memory and semaphore
    sharedmem_detach(pLoadBalancingAnswerInterfaceBase);
    sharedmem_detach(pGlobalVariableInterfaceBase);
    sharedmem_destroy(shmIDGlobalVariableInterface);
    semaphore_destroy(semIDGlobalVariableInterface);
    
    return UA_STATUSCODE_GOOD;
}