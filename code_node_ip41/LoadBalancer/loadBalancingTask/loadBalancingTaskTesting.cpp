#define _XOPEN_SOURCE 500

#include <iostream>
#include <signal.h>
#include <sys/time.h>
#include <vector>
#define SHM_SHAREDMEMORY_SIZE 13
#define SHM_LOADBALANCINGINTERFACE_SIZE 14


using namespace std;


int main(void) {
    
    printf("am i internally = %d \n", amIInternally());
        
    

    
    
    return 0;
}