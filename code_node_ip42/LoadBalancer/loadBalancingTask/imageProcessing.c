//imageProcessing.c
#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main (void) {
        
        int waitmicroSec = 0;
        srand(time(NULL));
        while(1) {
            waitmicroSec = rand()%2000000;
            if(waitmicroSec < 1000000) {
                continue;
            }   
            usleep(waitmicroSec);
            break;
        }
        for (int i = 0; i < 100000; i++) {
            waitmicroSec = rand()%15;
            if(waitmicroSec == 0) {
                continue;
            }
            usleep(waitmicroSec);
        }
        for (int i = 0; i < 1000; i++) {
            waitmicroSec = rand()%5000;
            if(waitmicroSec == 0) {
                continue;
            }
            usleep(waitmicroSec);
        }
        
        //printf("[imageProcessing]: finished \n");
        
        return 0;
}