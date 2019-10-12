#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

int main (void) {
    
    srand(time(NULL));
    float hit = 0.0;
    
    for (int i = 0; i < 1000000; i++) {
        for(int j = 0; j < 45; j++) {
            if (rand()%45 == j) {
                hit = hit + 1.0;
                break;
            }
        }
    }
    printf("empirische Wahrscheinlichkeit von %f \n", hit/1000000);
    
    
    return 0;
}