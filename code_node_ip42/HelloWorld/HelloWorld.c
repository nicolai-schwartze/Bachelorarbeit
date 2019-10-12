#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    _Bool b = NULL;
	
    FILE *export_file = NULL;        //declare pointers
    FILE *IO_direction = NULL;
    char str1[] = "low";
    char str2[] = "high";
    char str[] = "23";                       //value to pass to export file
    export_file = fopen ("/sys/class/gpio/export", "w");
    fwrite (str, 1, sizeof(str), export_file);
    fclose (export_file);

	for (int i=0; i<10; i++) {        //blink LED 10 times
		IO_direction = fopen ("/sys/class/gpio/gpio23/direction", "w");
        fwrite (str2, 1, sizeof(str1), IO_direction);   //set the pin to HIGH
        fclose (IO_direction);
        usleep (1000000);
        printf("high\n");

        IO_direction = fopen ("/sys/class/gpio/gpio23/direction", "w");
        fwrite (str1, 1, sizeof(str1), IO_direction);   //set the pin to LOW
        fclose (IO_direction);
        usleep (1000000);
        printf("low\n");
	}

    export_file = fopen ("/sys/class/gpio/unexport", "w");   //remove the mapping
    fwrite (str, 1, sizeof(str), export_file);
    fclose (export_file);

}

