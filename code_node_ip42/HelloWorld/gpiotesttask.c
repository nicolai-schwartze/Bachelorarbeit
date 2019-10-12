#include "ipclib.h"
#include <stdio.h>
#include <stdlib.h>
#define SHM_SHAREDMEMORY_SIZE 10


int main (void) {

    key_t semKeySharedMemory = 1111;
    int semIDSharedMemory = semaphore_init(semKeySharedMemory); 
    
    key_t shmKeySharedMemory = 1112;
    int shmIDSharedMemory = sharedmem_init(shmKeySharedMemory, SHM_SHAREDMEMORY_SIZE);
    _Bool *pSharedMemoryBase = sharedmem_attach(shmIDSharedMemory);
    
    for (int i = 0; i <= 1500; i++) {
        semaphore_operation(semIDSharedMemory, -1);
        *(pSharedMemoryBase +8) = i%2;
        printf("*pSharedMemoryBase + 8 = %d \n", *(pSharedMemory))
        semaphore_operation(semIDSharedMemory, 1);
        sleep(10);
    }
    
    return 0;
}