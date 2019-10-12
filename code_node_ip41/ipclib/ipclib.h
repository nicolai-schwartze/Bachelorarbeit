#ifndef __IPCLIB_H_INCLUDED__
#define __IPCLIB_H_INCLUDED__

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <signal.h>
#include <errno.h>

#define LOCK       -1
#define UNLOCK      1
#define PERM        0666

//semaphores
int semaphore_init (key_t key);
int semaphore_operation (int semid, int op); 
int semaphore_destroy (int semid);

//shared memory
int sharedmem_init (key_t key, int byte); 
void *sharedmem_attach (int shmid); 
int sharedmem_detach (void *ptr); 
int sharedmem_destroy (int shmid);

//forking, checking and deliting processes
int checkPID (int pid);
int callTask (char *process);
void deleteZombie (int sig);

#endif