#define _XOPEN_SOURCE 500 // for POSIX Standard 2008

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define LOCK       -1
#define UNLOCK      1
#define PERM        0666

int semaphore_init (key_t key);
int semaphore_operation (int semid, int op); 
int semaphore_destroy (int semid); 
int sharedmem_init (key_t key, int byte); 
void *sharedmem_attach (int shmid); 
int sharedmem_detach (void *ptr); 
int sharedmem_destroy (int shmid);