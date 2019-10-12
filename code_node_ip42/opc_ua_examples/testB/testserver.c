#include <signal.h>
#include <stdio.h>
#include "open62541.h"
#include <unistd.h>

UA_Boolean running = true;
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

int *serverReadServerVar(UA_Server *server) {
    UA_Variant value; 
    UA_Variant_init(&value);
    const UA_NodeId nodeId = UA_NODEID_STRING(1, "the.answer");
    UA_StatusCode retval = UA_Server_readValue(server, nodeId, &value);
    int *dataPointer = value.data;
    return dataPointer;
    
}

int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_ServerConfig *config = UA_ServerConfig_new_default();
    UA_Server *server = UA_Server_new(config);
    
    UA_StatusCode retval;

    UA_VariableAttributes attr = UA_VariableAttributes_default;
    UA_Int32 myInteger = 42;
    UA_Variant_setScalar(&attr.value, &myInteger, &UA_TYPES[UA_TYPES_INT32]);
    attr.description = UA_LOCALIZEDTEXT("en-US","the answer");
    attr.displayName = UA_LOCALIZEDTEXT("en-US","the answer");
    attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;

    /* Add the variable node to the information model */
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "the.answer");
    UA_QualifiedName myIntegerName = UA_QUALIFIEDNAME(1, "the answer");
    UA_NodeId parentNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    UA_NodeId parentReferenceNodeId = UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES);
    UA_Server_addVariableNode(server, myIntegerNodeId, parentNodeId,
                              parentReferenceNodeId, myIntegerName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, NULL, NULL);
    
    UA_Server_run_startup(server);
    int *serverVar = NULL;
    
    for(int i = 0; i < 100; i++) {
        serverVar = serverReadServerVar(server);
        printf("before printf \n");
        printf("%i \n", *serverVar);
        printf("after printf \n");
        UA_Server_run_iterate(server, 1);
        sleep(2);
        if(running == false) {
            break;
        }
    }
    
    UA_Server_run_shutdown(server);
    
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return (int)retval;
}

