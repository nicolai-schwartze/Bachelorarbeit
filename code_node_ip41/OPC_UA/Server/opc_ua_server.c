//opc_ua_server.c
//for compiling: 
//gcc --std=c99 -D_XOPEN_SOURCE=500 opc_ua_server.c /var/lib/cloud9/c_cpp_projects/opclib/open62541.c /var/lib/cloud9/c_cpp_projects/ipclib/ipclib.c -o opc_ua_server

//functionality: write communication vars from ther server to the load balancing interface
//these tasks are called and executed by ther load balancer

#include <signal.h>
#include <stdio.h>
#include "/var/lib/cloud9/c_cpp_projects/opclib/open62541.h"
#include "/var/lib/cloud9/c_cpp_projects/ipclib/ipclib.h"
#include <unistd.h>

//load balancing interface size 
#define SHM_LOADBALANCINGINTERFACE_SIZE 14

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"



UA_Boolean running = true;

//signal handler
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

//write boolean variables from C to the server
void writeVariableBool(UA_Server *server, UA_Boolean serverVar, char *stringNodeID) {
    UA_NodeId myBooleanNodeId = UA_NODEID_STRING(1, stringNodeID);

    /* Write a different boolean value */
    UA_Boolean myBoolean = serverVar;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myBoolean, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_Server_writeValue(server, myBooleanNodeId, myVar);

    /* Set the status code of the value to an error code. The function
     * UA_Server_write provides access to the raw service. The above
     * UA_Server_writeValue is syntactic sugar for writing a specific node
     * attribute with the write service. */
    UA_WriteValue wv;
    UA_WriteValue_init(&wv);
    wv.nodeId = myBooleanNodeId;
    wv.attributeId = UA_ATTRIBUTEID_VALUE;
    wv.value.status = UA_STATUSCODE_BADNOTCONNECTED;
    wv.value.hasStatus = true;
    UA_Server_write(server, &wv);

    /* Reset the variable to a good statuscode with a value */
    wv.value.hasStatus = false;
    wv.value.value = myVar;
    wv.value.hasValue = true;
    UA_Server_write(server, &wv);
} 

//write integer variables from C to server
void writeVariableInteger(UA_Server *server, UA_Int32 serverVar, char *stringNodeID) {
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, stringNodeID);

    /* Write a different boolean value */
    UA_Int32 myInteger = serverVar;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, myIntegerNodeId, myVar);

    /* Set the status code of the value to an error code. The function
     * UA_Server_write provides access to the raw service. The above
     * UA_Server_writeValue is syntactic sugar for writing a specific node
     * attribute with the write service. */
    UA_WriteValue wv;
    UA_WriteValue_init(&wv);
    wv.nodeId = myIntegerNodeId;
    wv.attributeId = UA_ATTRIBUTEID_VALUE;
    wv.value.status = UA_STATUSCODE_BADNOTCONNECTED;
    wv.value.hasStatus = true;
    UA_Server_write(server, &wv);

    /* Reset the variable to a good statuscode with a value */
    wv.value.hasStatus = false;
    wv.value.value = myVar;
    wv.value.hasValue = true;
    UA_Server_write(server, &wv);
} 

//read a server var to c variables
int *serverReadServerVarInteger(UA_Server *server, char *stringNodeID) {
    UA_Variant value; 
    UA_Variant_init(&value);
    const UA_NodeId nodeId = UA_NODEID_STRING(1, stringNodeID);
    UA_StatusCode retval = UA_Server_readValue(server, nodeId, &value);
    int *dataPointer = value.data;
    return dataPointer;
}


int main(void) {
    //signal handlers
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);
    
    //set server configurations
    UA_ServerConfig *config = UA_ServerConfig_new_default();
    config->maxSessionTimeout = 60.0*60.0*1000.0;
    config->maxSecurityTokenLifetime = 60*60*1000;
    UA_Server *server = UA_Server_new(config);
    
    UA_StatusCode retval;
    
    //initialise semaphore for load balancing interface
    key_t semKeyLoadBalancingInterface = 3331;
    int semIDLoadBalancingInterface = semaphore_init(semKeyLoadBalancingInterface);
    
    //initialise shared memory load balancing task interface
    key_t shmKeyLoadBalancingInterface = 3332;
    int shmIDLoadBalancingInterface = sharedmem_init(shmKeyLoadBalancingInterface, SHM_LOADBALANCINGINTERFACE_SIZE);
    int *pLoadBalancingInterfaceBase = (int*) sharedmem_attach(shmIDLoadBalancingInterface);
    
    UA_VariableAttributes tasksToCallAttr[7];
    
    //initialise with zero
    for (int i = 0; i < 7; i++) {
        tasksToCallAttr[i] = UA_VariableAttributes_default;
    }
    
    UA_Int32 tasksToCallInteger[7] = {0, 0, 0, 0, 0, 0, 0};
    //string name for nodeID
    char name[15] = "tasks Server X";
    
    //add variables to the server
    for (int i = 0; i < 7; i++) {
        UA_Variant_setScalar(&(tasksToCallAttr[i].value), &(tasksToCallInteger[i]), &UA_TYPES[UA_TYPES_INT32]);
        
        name[13] = (char)(i + 48);
        
        tasksToCallAttr[i].description = UA_LOCALIZEDTEXT("en-US", name);
        tasksToCallAttr[i].displayName = UA_LOCALIZEDTEXT("en-US", name);
        tasksToCallAttr[i].dataType = UA_TYPES[UA_TYPES_INT32].typeId;
        tasksToCallAttr[i].accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        
        UA_NodeId tasksNodeID = UA_NODEID_STRING(1, name);
        UA_QualifiedName tasksToCallName = UA_QUALIFIEDNAME(1, name);
        UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
        UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
        UA_Server_addVariableNode(server, tasksNodeID, parentNodeId, parentReferenceNodeId, tasksToCallName, UA_NODEID_NULL, tasksToCallAttr[i], NULL, NULL);
    }
    
    //add heartbeat to the server - session does not time out
    UA_VariableAttributes heartBeatAttr;
    heartBeatAttr = UA_VariableAttributes_default;
    UA_Boolean heartBeatBool = 0;
    UA_Variant_setScalar(&heartBeatAttr.value, &heartBeatBool, &UA_TYPES[UA_TYPES_BOOLEAN]);
    heartBeatAttr.description = UA_LOCALIZEDTEXT("en_US","heartBeat");
    heartBeatAttr.displayName = UA_LOCALIZEDTEXT("en_US","heartBeat");
    heartBeatAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    heartBeatAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId heartBeatNodeId = UA_NODEID_STRING(1, "heartBeat");
    UA_QualifiedName myHeartBeatName = UA_QUALIFIEDNAME(1, "heartBeat");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, heartBeatNodeId, parentNodeId, parentReferenceNodeId, myHeartBeatName, UA_NODEID_NULL, heartBeatAttr, NULL, NULL);
    
    _Bool heartBeat = 0;
    
    //start server
    UA_Server_run_startup(server);
    
    while(running) {
        
        //write heartbeat
        if(heartBeat) {
            heartBeat = 0;
        }
        else {
            heartBeat = 1;
        }
                
        writeVariableBool(server, heartBeat, "heartBeat");
        
        
        //write values from comm to UA Bool
        for(int i = 0; i < 7; i++) {
            name[13] = (char)(i + 48); 
            int *intermediateVariable = serverReadServerVarInteger(server, name);
            tasksToCallInteger[i] = *intermediateVariable;
        }
        
        //write variables from intermediate placeholder to shared memory
        semaphore_operation(semIDLoadBalancingInterface, LOCK);
        for (int i = 0; i < 7; i++) {
            pLoadBalancingInterfaceBase[i] = pLoadBalancingInterfaceBase[i] + tasksToCallInteger[i];
            name[13] = (char)(48 + i);
            writeVariableInteger(server, 0, name);
        }
        semaphore_operation(semIDLoadBalancingInterface, UNLOCK);
        
        //iterate with every loop
        UA_Server_run_iterate(server, 1);
        sleep(1);
    }
    
    //shutdown and clean-up
    UA_Server_run_shutdown(server);
    
    sharedmem_detach(pLoadBalancingInterfaceBase);
    
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return (int)retval;
}