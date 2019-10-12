
//gcc -std=c99 blinky.c /var/lib/cloud9/c_cpp_projects/ipclib/ipclib.c -o blinky

#define _XOPEN_SOURCE 500

#include "/var/lib/cloud9/c_cpp_projects/ipclib/ipclib.h"
#include <stdio.h>
#include <stdlib.h>
#define SHM_SHAREDMEMORY_SIZE 13
#define SHM_GLOBALVARABLEINTERFACE_SIZE 6

_Bool amIInternally (void) {
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


int main (void) {

    key_t semKeySharedMemory = 1111;
    int semIDSharedMemory = semaphore_init(semKeySharedMemory); 
    
    key_t shmKeySharedMemory = 1112;
    int shmIDSharedMemory = sharedmem_init(shmKeySharedMemory, SHM_SHAREDMEMORY_SIZE);
    _Bool *pSharedMemoryBase = sharedmem_attach(shmIDSharedMemory);
    
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
    
    
    if(amIInternally()) {
    
        for (int i = 0; i <= 10; i++) {
            semaphore_operation(semIDSharedMemory, -1);
            *(pSharedMemoryBase +8) = 1;
            semaphore_operation(semIDSharedMemory, 1);
            sleep(30);
            semaphore_operation(semIDSharedMemory, -1);
            *(pSharedMemoryBase +8) = 0;
            semaphore_operation(semIDSharedMemory, 1);
            sleep(30);
            
        }
    }
    else {
        for (int i = 0; i <= 10; i++) {
            semaphore_operation(semIDGlobalVariableInterface, LOCK);
            *(pGlobalVariableInterfaceOutputSetter) = 1;
            semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
            sleep(30);
            semaphore_operation(semIDGlobalVariableInterface, LOCK);
            *(pGlobalVariableInterfaceOutputSetter) = 0;
            semaphore_operation(semIDGlobalVariableInterface, UNLOCK);
            sleep(30);
            
        }
    }
    
    sharedmem_detach(pSharedMemoryBase);
    
    //printf("[blinky]: finished \n");
    return 0;
    
}