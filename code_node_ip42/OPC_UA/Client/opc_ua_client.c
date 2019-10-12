//opc_ua_client.c
//for compiling: 
//gcc -std=c99 -D_XOPEN_SOURCE=500 opc_ua_client.c /var/lib/cloud9/c_cpp_projects/opclib/open62541.c /var/lib/cloud9/c_cpp_projects/ipclib/ipclib.c -o opc_ua_client

//short functionality summary: takes the external executed tasks form the load balancer and writes them to the opc_ua server on the other node

#include <stdio.h>
#include "/var/lib/cloud9/c_cpp_projects/opclib/open62541.h"
#include <unistd.h>
#include "/var/lib/cloud9/c_cpp_projects/ipclib/ipclib.h"

//for colouring the outputs
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//size of shared memory
#define SHM_LOADBALANCINGINTERFACE_SIZE 28


_Bool isRunning = 1;

//signal function
void controlCSig(int sig) {
    isRunning = 0;
} 

int main(void) {
    
    //signal handler
    signal(SIGINT, controlCSig);
    
    //initialise semaphore for load balancing interface
    key_t semKeyLoadBalancingInterface = 3331;
    int semIDLoadBalancingInterface = semaphore_init(semKeyLoadBalancingInterface);
    
    //initialise shared memory load balancing task interface
    key_t shmKeyLoadBalancingInterface = 3332;
    int shmIDLoadBalancingInterface = sharedmem_init(shmKeyLoadBalancingInterface, SHM_LOADBALANCINGINTERFACE_SIZE);
    int *pLoadBalancingInterfaceBase = (int*) sharedmem_attach(shmIDLoadBalancingInterface);
    
    //name for node id
    char name[15] = "tasks Server X";
    
    //set configurations of ua client
    UA_ClientConfig conf = UA_ClientConfig_default;
    conf.secureChannelLifeTime = 60*60*1000;
    UA_Client *client = UA_Client_new(conf);
    
    //try to connect until connected
    while (1) {
    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://10.0.0.41:4840");
        if(retval != UA_STATUSCODE_GOOD) {
            printf(ANSI_COLOR_RED "[opc_ua_client]: server 10.0.0.41:4840 not responding \n" ANSI_COLOR_RESET);
            return (int)retval;
        }
        else {
            break;
        }
    }
    printf(ANSI_COLOR_GREEN "[opc_ua_client]: connected to server 10.0.0.41:4840 \n" ANSI_COLOR_RESET);
    

    //for transfering the values to the server
    UA_Variant value; 
    UA_Variant_init(&value);
    
    //client main loop, do until reached ctrl + c
    while(isRunning) {
        semaphore_operation(semIDLoadBalancingInterface, LOCK);
        //cycle through all possible tasks
        for (int i = 0; i < 7; i++) {
            if (pLoadBalancingInterfaceBase[i] != 0) {
                UA_Variant value; 
                UA_Variant_init(&value);
                UA_Variant value2;
                UA_Variant_init(&value2);
                name[13] = (char)(i + 48);
                const UA_NodeId nodeId = UA_NODEID_STRING(1, name);
                UA_Client_readValueAttribute(client, nodeId, &value2);
                UA_Int32 intermediateTaskInteger = pLoadBalancingInterfaceBase[i] + *(UA_Int32 *)value2.data;
                UA_Variant_setScalar(&value, &intermediateTaskInteger, &UA_TYPES[UA_TYPES_INT32]);
                UA_Client_writeValueAttribute(client, nodeId, &value);
                pLoadBalancingInterfaceBase[i] = 0;
            }
            else {
                continue;
            }
        }
        semaphore_operation(semIDLoadBalancingInterface, UNLOCK);
        
        //break the loop it the connection is lost
        UA_StatusCode testConnection = UA_Client_connect(client, "opc.tcp://10.0.0.41:4840");
        if(testConnection != UA_STATUSCODE_GOOD) {
            printf(ANSI_COLOR_RED "[opc_ua_client]: server 10.0.0.41:4840 connection lost \n" ANSI_COLOR_RESET);
            isRunning = 0;
        }
        
        //check the heart beat while running
        //so the session does not timeout
        if(isRunning) {
            UA_Variant value;
            UA_Variant_init(&value);
            UA_StatusCode retvalTemp = UA_Client_connect(client, "opc.tcp://10.0.0.41:4840");
            const UA_NodeId heartBeatNID = UA_NODEID_STRING(1, "heartBeat");
            retvalTemp = UA_Client_readValueAttribute(client, heartBeatNID, &value);
        }
        
        sleep(1);
    }
    
    // Clean up
    //disconnect client
    UA_Client_delete(client); 
    
    //clear shared memory and semaphore
    sharedmem_detach(pLoadBalancingInterfaceBase);
    
    return UA_STATUSCODE_GOOD;
}