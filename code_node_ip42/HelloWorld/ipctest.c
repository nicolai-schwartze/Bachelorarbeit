#include "ipclib.h"
#include <stdio.h>
#include <stdlib.h>
#define SHM_SHAREDMEMORY_SIZE 13


int main (void) {

    key_t semKeySharedMemory = 1111;
    int semIDSharedMemory = semaphore_init(semKeySharedMemory); 
    
    key_t shmKeySharedMemory = 1112;
    int shmIDSharedMemory = sharedmem_init(shmKeySharedMemory, SHM_SHAREDMEMORY_SIZE);
    _Bool *pSharedMemoryBase = sharedmem_attach(shmIDSharedMemory);
    
    for (int i = 0; i <= 15; i++) {
        semaphore_operation(semIDSharedMemory, -1);
        *(pSharedMemoryBase + 11) = 1;
        semaphore_operation(semIDSharedMemory, 1);
        printf("*(pSharedMemoryBase + 11) = %d \n", *(pSharedMemoryBase + 11));
        sleep(15);
        semaphore_operation(semIDSharedMemory, -1);
        *(pSharedMemoryBase + 11) = 0;
        semaphore_operation(semIDSharedMemory, 1);
        printf("*(pSharedMemoryBase + 11) = %d \n", *(pSharedMemoryBase + 11));
        sleep(15);
    }
    
    return 0;
}