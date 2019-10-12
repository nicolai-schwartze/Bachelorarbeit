#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (void) {
    int nanosec = 50000;
    char fileCont [] = "0.00";
    while(1) {
        FILE *loadavg = fopen("/proc/loadavg", "r");
        if(NULL == loadavg) {
            printf("[loadController]: /proc/loadavg not readable\n");
            return EXIT_FAILURE;
        }
        for(int i = 0; i < 4; i++) {
            fileCont[i] = fgetc(loadavg);
        }
        fclose(loadavg);
        double load = ((double)fileCont[0] - 48.0) + ((0.1)*((double)fileCont[2] - 48.0)) + ((0.01) * ((double)fileCont[3] - 48.0));
        printf("[loadController]: loadavg = %f \n", load);
        if ((load >= 1.00) || (nanosec < 200)) {
            nanosec = nanosec + 100;
        }
        else{
            nanosec = nanosec - 100;
        }
        printf("[loadController]: sleep = %i µs \n", nanosec);
        usleep(nanosec);
    }

return EXIT_SUCCESS;

}
