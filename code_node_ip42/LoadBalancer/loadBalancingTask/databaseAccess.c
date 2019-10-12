//databaseAccess.c
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>



int main (void) {
        int waitmicroSec = 123456;
        srand(time(NULL));
        for (int i = 0; i < 10000; i++) {
                waitmicroSec = rand()%1000;
                if (waitmicroSec == 0) {
                        continue;
                }
                usleep(waitmicroSec);
        }
        while(1) {
                waitmicroSec = rand()%10000000;
                if(waitmicroSec < 1000000) {
                        continue;
                }
                usleep(waitmicroSec);
                break;
        }
        for (int i = 0; i < 10000; i++) {
                waitmicroSec = rand()%550;
                if(waitmicroSec == 0) {
                        continue;
                }
                usleep(waitmicroSec);
        }
        //printf("[databaseAccess]: finished \n");
        return 0;

}
