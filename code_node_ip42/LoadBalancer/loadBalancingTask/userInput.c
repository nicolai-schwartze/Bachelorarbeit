//userInput.c
#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main (void) {
        
        int waitmicroSec = 0;
        srand(time(NULL));
        int turns = 0;
        while(1) {
            turns = rand()%20;
            if (turns < 8) {
                continue;
            }
            break;
        }
        
        for (int i = 0; i <= turns; i++) {
            while(1) {
                waitmicroSec = rand()%10000000;
                if(waitmicroSec < 5000000) {
                    continue;
                }
                usleep(waitmicroSec);
                break;
            }
            for (int i = 0; i < 1000; i++) {
                waitmicroSec = rand()%8000;
                if(waitmicroSec == 0) {
                    continue;
                }
                usleep(waitmicroSec);
            }
            
        }
        
        //printf("[userInput]: finished \n");
        
        return 0;
}