//opc_ua_server.c
//for compiling: 
//gcc --std=c99 -D_XOPEN_SOURCE=500 opc_ua_server.c /var/lib/cloud9/c_cpp_projects/opclib/open62541.c /var/lib/cloud9/c_cpp_projects/ipclib/ipclib.c -o opc_ua_server

#include <signal.h>
#include <stdio.h>
#include "/var/lib/cloud9/c_cpp_projects/opclib/open62541.h"
#include "/var/lib/cloud9/c_cpp_projects/ipclib/ipclib.h"
#include <unistd.h>

//colours in standard output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//shared memory sizes
#define SHM_LOADBALANCINGINTERFACE_SIZE 14
#define SHM_GLOBALVARABLEINTERFACE_SIZE 6
#define SHM_LOADBALANCING_ANSWER_INTERFACE_SIZE 28

UA_Boolean running = true;


//signal handler
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

//function for writeing an boolean value form somewhere in the server to the comm variables in the server
void writeVariableBool(UA_Server *server, UA_Boolean serverVar, char *stringNodeID) {
    UA_NodeId myBooleanNodeId = UA_NODEID_STRING(1, stringNodeID);

    
    UA_Boolean myBoolean = serverVar;
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myBoolean, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_Server_writeValue(server, myBooleanNodeId, myVar);

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

//function for writeing an integer value form somewhere in the server to the comm variables in the server
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

//read values form opc ua comm variables to normal c boolen variables
_Bool *serverReadServerVarBool(UA_Server *server, char *stringNodeID) {
    UA_Variant value; 
    UA_Variant_init(&value);
    const UA_NodeId nodeId = UA_NODEID_STRING(1, stringNodeID);
    UA_StatusCode retval = UA_Server_readValue(server, nodeId, &value);
    _Bool *dataPointer = value.data;
    return dataPointer;
}

//read values form opc ua comm variables to normal c integer variables; watch out for sizeof(int)!!
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
    
    //server configuration
    UA_ServerConfig *config = UA_ServerConfig_new_default();
    config->maxSessionTimeout = 3600000.0;
    config->maxSecurityTokenLifetime = 60*60*1000;
    UA_Server *server = UA_Server_new(config);
    
    UA_StatusCode retval;
    
    //initialise semaphores for global bariable interface
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
    semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
    
    //initialise semaphore for load balancing answer interface
    key_t semKeyLoadBalancingAnswerInterface = 4441;
    int semIDLoadBalancingAnswerInterface = semaphore_init(semKeyLoadBalancingAnswerInterface);
    
    //initialise shared memory load balancing answer interface
    key_t shmKeyLoadBalancingAnserInterface = 4442;
    int shmIDLoadBalancingAnswerInterface = sharedmem_init(shmKeyLoadBalancingAnserInterface, SHM_LOADBALANCING_ANSWER_INTERFACE_SIZE);
    int *pLoadBalancingAnswerInterfaceBase = (int*) sharedmem_attach(shmIDLoadBalancingAnswerInterface);
    
    semaphore_operation(semIDLoadBalancingAnswerInterface, LOCK);
    for (int i = 0; i < 7; i++) {
        pLoadBalancingAnswerInterfaceBase[i] = 0;
    }
    semaphore_operation(semIDLoadBalancingAnswerInterface, UNLOCK);
    
    UA_VariableAttributes tasksFinishedAttr[7];
    
    for (int i = 0; i < 7; i++) {
        tasksFinishedAttr[i] = UA_VariableAttributes_default;
    }
    
    UA_Int32 tasksFinishedInteger[7] = {0, 0, 0, 0, 0, 0, 0};
    //name string for node id
    char name[] = "answer Server X";
    
    //cycle through all variables and initialise
    for (int i = 0; i < 7; i++) {
        UA_Variant_setScalar(&(tasksFinishedAttr[i].value), &(tasksFinishedInteger[i]), &UA_TYPES[UA_TYPES_INT32]);
        
        name[14] = (char)(i + 48);
        
        tasksFinishedAttr[i].description = UA_LOCALIZEDTEXT("en-US", name);
        tasksFinishedAttr[i].displayName = UA_LOCALIZEDTEXT("en-US", name);
        tasksFinishedAttr[i].dataType = UA_TYPES[UA_TYPES_INT32].typeId;
        tasksFinishedAttr[i].accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        
        //and add the variable to the opc_ua variable
        UA_NodeId tasksNodeID = UA_NODEID_STRING(1, name);
        UA_QualifiedName tasksToCallName = UA_QUALIFIEDNAME(1, name);
        UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
        UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
        UA_Server_addVariableNode(server, tasksNodeID, parentNodeId, parentReferenceNodeId, tasksToCallName, UA_NODEID_NULL, tasksFinishedAttr[i], NULL, NULL);
    }
    
    //global Variable Input Setter
    UA_VariableAttributes globalVarInputSetterAttr;
    globalVarInputSetterAttr = UA_VariableAttributes_default;
    UA_Boolean globalVarInputSetterBool = 0;
    
    UA_Variant_setScalar(&(globalVarInputSetterAttr.value), &(globalVarInputSetterBool), &UA_TYPES[UA_TYPES_BOOLEAN]);
    
    globalVarInputSetterAttr.description = UA_LOCALIZEDTEXT("en_US", "globalVarInputSetter");
    globalVarInputSetterAttr.displayName = UA_LOCALIZEDTEXT("en_US", "globalVarInputSetter");
    globalVarInputSetterAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    globalVarInputSetterAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId globalVarInputSetterNodeId = UA_NODEID_STRING(1, "globalVarInputSetter");
    UA_QualifiedName myglobalVarInputSetterName = UA_QUALIFIEDNAME(1, "globalVarInputSetter");
    UA_NodeId parentNodeIdgVIS = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeIdgVIS = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, globalVarInputSetterNodeId, parentNodeIdgVIS, parentReferenceNodeIdgVIS, myglobalVarInputSetterName, UA_NODEID_NULL, globalVarInputSetterAttr, NULL, NULL);
    
    
    //global Variable Output Setter
    UA_VariableAttributes globalVarOutputSetterAttr;
    globalVarOutputSetterAttr = UA_VariableAttributes_default;
    UA_Boolean globalVarOutputSetterBool = 0;
    
    UA_Variant_setScalar(&(globalVarOutputSetterAttr.value), &(globalVarOutputSetterBool), &UA_TYPES[UA_TYPES_BOOLEAN]);
    
    globalVarOutputSetterAttr.description = UA_LOCALIZEDTEXT("en_US", "globalVarOutputSetter");
    globalVarOutputSetterAttr.displayName = UA_LOCALIZEDTEXT("en_US", "globalVarOutputSetter");
    globalVarOutputSetterAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    globalVarOutputSetterAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId globalVarOutputSetterNodeId = UA_NODEID_STRING(1, "globalVarOutputSetter");
    UA_QualifiedName myglobalVarOutputSetterName = UA_QUALIFIEDNAME(1, "globalVarOutputSetter");
    UA_NodeId parentNodeIdgVOS = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeIdgVOS = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, globalVarOutputSetterNodeId, parentNodeIdgVOS, parentReferenceNodeIdgVOS, myglobalVarOutputSetterName, UA_NODEID_NULL, globalVarOutputSetterAttr, NULL, NULL);
    
    
    //global Variable Variable Setter
    UA_VariableAttributes globalVarVariableSetterAttr;
    globalVarVariableSetterAttr = UA_VariableAttributes_default;
    UA_Boolean globalVarVariableSetterBool = 0;
    
    UA_Variant_setScalar(&(globalVarVariableSetterAttr.value), &(globalVarVariableSetterBool), &UA_TYPES[UA_TYPES_BOOLEAN]);
    
    globalVarVariableSetterAttr.description = UA_LOCALIZEDTEXT("en_US", "globalVarVariableSetter");
    globalVarVariableSetterAttr.displayName = UA_LOCALIZEDTEXT("en_US", "globalVarVariableSetter");
    globalVarVariableSetterAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    globalVarVariableSetterAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId globalVarVariableSetterNodeId = UA_NODEID_STRING(1, "globalVarVariableSetter");
    UA_QualifiedName myglobalVarVariableSetterName = UA_QUALIFIEDNAME(1, "globalVarVariableSetter");
    UA_NodeId parentNodeIdgVVS = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeIdgVVS = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, globalVarVariableSetterNodeId, parentNodeIdgVVS, parentReferenceNodeIdgVVS, myglobalVarVariableSetterName, UA_NODEID_NULL, globalVarVariableSetterAttr, NULL, NULL);
    
    
    //global Variable Input Getter
    UA_VariableAttributes globalVarInputGetterAttr;
    globalVarInputGetterAttr = UA_VariableAttributes_default;
    UA_Boolean globalVarInputGetterBool = 0;
    
    UA_Variant_setScalar(&(globalVarInputGetterAttr.value), &(globalVarInputGetterBool), &UA_TYPES[UA_TYPES_BOOLEAN]);
    
    globalVarInputGetterAttr.description = UA_LOCALIZEDTEXT("en_US", "globalVarInputGetter");
    globalVarInputGetterAttr.displayName = UA_LOCALIZEDTEXT("en_US", "globalVarInputGetter");
    globalVarInputGetterAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    globalVarInputGetterAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId globalVarInputGetterNodeId = UA_NODEID_STRING(1, "globalVarInputGetter");
    UA_QualifiedName myglobalVarInputGetterName = UA_QUALIFIEDNAME(1, "globalVarInputGetter");
    UA_NodeId parentNodeIdgVIG = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeIdgVIG = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, globalVarInputGetterNodeId, parentNodeIdgVIG, parentReferenceNodeIdgVIG, myglobalVarInputGetterName, UA_NODEID_NULL, globalVarInputGetterAttr, NULL, NULL);
    
    
    //global Variable Output Getter
    UA_VariableAttributes globalVarOutputGetterAttr;
    globalVarOutputGetterAttr = UA_VariableAttributes_default;
    UA_Boolean globalVarOutputGetterBool = 0;
    
    UA_Variant_setScalar(&(globalVarOutputGetterAttr.value), &(globalVarOutputGetterBool), &UA_TYPES[UA_TYPES_BOOLEAN]);
    
    globalVarOutputGetterAttr.description = UA_LOCALIZEDTEXT("en_US", "globalVarOutputGetter");
    globalVarOutputGetterAttr.displayName = UA_LOCALIZEDTEXT("en_US", "globalVarOutputGetter");
    globalVarOutputGetterAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    globalVarOutputGetterAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId globalVarOutputGetterNodeId = UA_NODEID_STRING(1, "globalVarOutputGetter");
    UA_QualifiedName myglobalVarOutputGetterName = UA_QUALIFIEDNAME(1, "globalVarOutputGetter");
    UA_NodeId parentNodeIdgVOG = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeIdgVOG = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, globalVarOutputGetterNodeId, parentNodeIdgVOG, parentReferenceNodeIdgVOG, myglobalVarOutputGetterName, UA_NODEID_NULL, globalVarOutputGetterAttr, NULL, NULL);
    
    
    //global Variable Variable Getter
    UA_VariableAttributes globalVarVariableGetterAttr;
    globalVarVariableGetterAttr = UA_VariableAttributes_default;
    UA_Boolean globalVarVariableGetterBool = 0;
    
    UA_Variant_setScalar(&(globalVarVariableGetterAttr.value), &(globalVarVariableGetterBool), &UA_TYPES[UA_TYPES_BOOLEAN]);
    
    globalVarVariableGetterAttr.description = UA_LOCALIZEDTEXT("en_US", "globalVarVariableGetter");
    globalVarVariableGetterAttr.displayName = UA_LOCALIZEDTEXT("en_US", "globalVarVariableGetter");
    globalVarVariableGetterAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    globalVarVariableGetterAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId globalVarVariableGetterNodeId = UA_NODEID_STRING(1, "globalVarVariableGetter");
    UA_QualifiedName myglobalVarVariableGetterName = UA_QUALIFIEDNAME(1, "globalVarVariableGetter");
    UA_NodeId parentNodeIdgVVG = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeIdgVVG = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, globalVarVariableGetterNodeId, parentNodeIdgVVG, parentReferenceNodeIdgVVG, myglobalVarVariableGetterName, UA_NODEID_NULL, globalVarVariableGetterAttr, NULL, NULL);
    
    
    
    //Heart Beat: Client reading this variable periodically prevents the server session to time out
    UA_VariableAttributes heartBeatAttr;
    heartBeatAttr = UA_VariableAttributes_default;
    UA_Boolean heartBeatBool = 0;
    
    UA_Variant_setScalar(&(heartBeatAttr.value), &(heartBeatBool), &UA_TYPES[UA_TYPES_BOOLEAN]);
    
    heartBeatAttr.description = UA_LOCALIZEDTEXT("en_US", "heartBeat");
    heartBeatAttr.displayName = UA_LOCALIZEDTEXT("en_US", "heartBeat");
    heartBeatAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    heartBeatAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    UA_NodeId heartBeatNodeId = UA_NODEID_STRING(1, "heartBeat");
    UA_QualifiedName myHeartBeatName = UA_QUALIFIEDNAME(1, "heartBeat");
    UA_NodeId parentNodeIdHB = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeIdHB = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, heartBeatNodeId, parentNodeIdHB, parentReferenceNodeIdHB, myHeartBeatName, UA_NODEID_NULL, heartBeatAttr, NULL, NULL);
    
    _Bool heartBeat = 0;
    
    UA_Server_run_startup(server);
    
    while(running) {
        
        //changing the Heart Beat Boolean periodically
        //actually not entirely sure if this is needed ... 
        if(heartBeat) {
            heartBeat = 0;
        }
        else {
            heartBeat = 1;
        }
        writeVariableBool(server, heartBeat, "heartBeat");
        
        //write variables from shared memory "gloabl Variable Interface" to server comm vars
        semaphore_operation(semIDGlobalVariableInterface, LOCK);
        writeVariableBool(server, *pGlobalVariableInterfaceInputGetter, "globalVarInputGetter");
        writeVariableBool(server, *pGlobalVariableInterfaceOutputGetter, "globalVarOutputGetter");
        writeVariableBool(server, *pGlobalVariableInterfaceVariableGetter, "globalVarVariableGetter");
        
        //set the shared memory variables from server vars
        _Bool *intermediateVariableOutput = serverReadServerVarBool(server, "globalVarOutputSetter");
        *pGlobalVariableInterfaceOutputSetter = *intermediateVariableOutput;
        
        _Bool *intermediateVariableVariable = serverReadServerVarBool(server, "globalVarVariableSetter");
        *pGlobalVariableInterfaceVariableSetter = *intermediateVariableVariable;
        semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
        
        //write values from comm to UA Bool
        for(int i = 0; i < 7; i++) {
            name[14] = (char)(i + 48); 
            int *intermediateVariable = serverReadServerVarInteger(server, name);
            tasksFinishedInteger[i] = *intermediateVariable;
            writeVariableInteger(server, 0, name);
        }
        
        //write server comm vars to load balancing answer interface
        semaphore_operation(semIDLoadBalancingAnswerInterface, LOCK);
        for (int i = 0; i < 7; i++) {
            pLoadBalancingAnswerInterfaceBase[i] = pLoadBalancingAnswerInterfaceBase[i] + tasksFinishedInteger[i];
            name[14] = (char)(48 + i);
        }
        //left from debugging
        //printf("pLoadBalancingAnswerInterfaceBase = {%i %i %i %i %i %i %i} \n", pLoadBalancingAnswerInterfaceBase[0], pLoadBalancingAnswerInterfaceBase[1], pLoadBalancingAnswerInterfaceBase[2], pLoadBalancingAnswerInterfaceBase[3], pLoadBalancingAnswerInterfaceBase[4], pLoadBalancingAnswerInterfaceBase[5], pLoadBalancingAnswerInterfaceBase[6]); 
        semaphore_operation(semIDLoadBalancingAnswerInterface, UNLOCK); 
        
        UA_Server_run_iterate(server, 1);
        sleep(1);
        
    }
    
    //shutting down server and shared memory
    UA_Server_run_shutdown(server);
    
    sharedmem_detach(pLoadBalancingAnswerInterfaceBase);
    
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return (int)retval;
}