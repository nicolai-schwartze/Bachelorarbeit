#include <stdio.h>
#include "open62541.h"
#include <unistd.h>

int main(void) {
    UA_Client *client = UA_Client_new(UA_ClientConfig_default);
    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4840");
    if(retval != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return (int)retval;
    }

    /* Read the value attribute of the node. UA_Client_readValueAttribute is a
     * wrapper for the raw read service available as UA_Client_Service_read. */
    UA_Variant value; /* Variants can hold scalar values and arrays of any type */
    UA_Variant_init(&value);

    /* NodeId of the variable holding the current time */
    for (int i = 0; i < 30; i++) {
        UA_Variant_setScalar(&value, &i, &UA_TYPES[UA_TYPES_INT32]);
        const UA_NodeId nodeId = UA_NODEID_STRING(1, "the.answer");
        UA_Client_writeValueAttribute(client, nodeId, &value);
        
        sleep(0.5);
    }
    
    /* Clean up */
    //UA_Variant_deleteMembers(&value);
    UA_Client_delete(client); /* Disconnects the client internally */
    return UA_STATUSCODE_GOOD;
}