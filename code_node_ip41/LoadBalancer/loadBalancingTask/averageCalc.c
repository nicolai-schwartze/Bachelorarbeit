//averageCalc.c
#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main (void) {
        
        int waitmicroSec = 0;
        int data[40320];
        srand(time(NULL));
        long double sum = 0.0;
        
        for (int i = 0; i < 40320; i++) {
            data[i] = (rand()%1000);
            usleep(10);
        }
        for (int i = 0; i < 40320; i++) {
            usleep(10);
            sum = sum + data[i];
        }
        //printf("[averageCalc]: mean = %f \n", ((float)sum)/40320);
        //printf("[averageCalc]: finished \n");
        
        return 0;
}