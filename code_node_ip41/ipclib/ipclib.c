
#include "ipclib.h"

//initialise shared memory form key
//if it does not exist, create
//if it already exists, don't create 
int semaphore_init (key_t key) {
    //test, if the semaphore already exists
    int semid = semget (key, 0, IPC_PRIVATE);
    if (semid < 0) {
        //if it does not exist, create
        //set permissions
        umask(0);
        //create one semaphore always use standard permissions 666
        semid = semget (key, 1, IPC_CREAT | IPC_EXCL | PERM);
        //if it did not work
        if (semid < 0) {
            printf ("[semaphore_init function]: semaphore could not be created \n");
            return -1;
        }
        //initialise semaphore with 1
        if (semctl (semid, 0, SETVAL, (int)1) == -1) {
            printf("[semaphore_init function]: semaphore could not be initialised \n");
            return -1;
        }
    }
    //retrun the id of the created semaphore 
    //don't loose it!
    return semid;
}

//function operates the semaphore with the semid
int semaphore_operation (int semid, int op) {
   //use defines LOCK or UNLOCK
   if((op != 1) && (op != -1)) {
       printf("[semaphore_operation function]: not a valid operation \n");
       return -1;
   }
   struct sembuf semaphore; 
   semaphore.sem_num = 0;
   semaphore.sem_op = op;
   semaphore.sem_flg = SEM_UNDO;
   //semaphore operation
   if(semop (semid, &semaphore, 1) == -1) {
       //if operation did not work correctly
      printf("[semaphore_operation function]: semaphore could not be operated \n");
      return -1;
   }
   return 1;
}

//destroying a semaphore (equal to ipcrm -s *semid*)
int semaphore_destroy (int semid) {
    if(semctl(semid, 0, IPC_RMID, 0) == -1) {
        printf("[semaphore_destroy function]: could not destoy semaphore %i \n", semid);
        return -1;
    }
    return 0;
    
}

//for creating a new shared mem
int sharedmem_init (key_t key, int byte) {
    if(byte <= 0) {
        printf("[sharedmem_init function]: not a valid size \n");
        return -1;
    }
    //test if the pointer does exist
    int shmid = shmget(key, byte, PERM);
    if (shmid == -1) {
        //if it does not exist, create
        shmid = shmget(key, byte, IPC_CREAT | PERM);
        if (shmid == -1) {
            //if creation did not work
            printf("[sharedmem_init function]: shared memory creation did not work \n");
            return -1;
        }
        //return the shared memory id
        else {
            return shmid;
        }
    }
    else {
        return shmid;
    }
}


//tries to attach the pointer to the shared memory segment
void *sharedmem_attach (int shmid) {
    void *ptr = shmat(shmid, 0, 0);
        if (ptr == (void *)-1) {
            printf("[sharedmem_attach function]: error with returning the pointer \n");
            return NULL;
        } 
        else {
            return ptr;
        }
    
}

//detach the shared memory segment
int sharedmem_detach (void *ptr) {
    if(shmdt(ptr) == -1) {
        printf("[sharedmem_detach function]: error with detaching the memory \n");
        return -1;
    }
    return 0;
}

//destroy shared memory segment
int sharedmem_destroy (int shmid) {
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        printf("[sharedmem_destroy function]: error with destroing the memory \n");
    }
    else {
        return 0;
    }
}

//check if a PID is currently taken
//verified with ps aux
int checkPID (int pid) {
    //send the signal 0 to the process
    int check = kill(pid, 0);
    //returns -1 and generates error 3 (ERSCH) meaning that this process does not exist
    if ((check == -1) && (errno == 3)) {
        return 1;
    }
    //if the process does exist: 
    else {
        return 0;
    }
}

//call any task, in particular the Load Balancing tasks
int callTask (char *process) {
    pid_t pid = fork();
    //ALTERNATIVE: system ("./path/task &");
    //if the pid is 0, that means that this process is the childprocess
    if (pid == 0) {
        //execute the process specified, any path or process
        execv(process, NULL);
        //exit the childprocess
        exit(127);
        //left form debugging
        printf("forked \"%s\" \n", process);
        return 0;
    }
    //if this is the parrent process, return the pid of the child process
    return pid;
}

//wait for a specific child
//enter pid - child is reaped and thus preventing zobmies
void deleteZombie (int PID) {
    waitpid(PID, NULL, 0);
}
