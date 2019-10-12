//websiteCall.c
#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main (void) {
    
        int waitmicroSec = 0;
        srand(time(NULL));
        for (int i = 0; i < 1000; i++) {
            waitmicroSec = rand()%5000;
            if(waitmicroSec == 0) {
                continue;
            }
            usleep(waitmicroSec);
        }
        while(1) {
            waitmicroSec = rand()%15000000;
            if (waitmicroSec < 5000000) {
                continue;
            }
            usleep(waitmicroSec);
            break;
        }
        for(int i = 0; i < 10000; i++) {
            waitmicroSec = rand()%3000;
            if(waitmicroSec == 0) {
                continue;
            }
            usleep(waitmicroSec);
        }
        //printf("[websiteCall]: finished \n");
        
        return 0;
}