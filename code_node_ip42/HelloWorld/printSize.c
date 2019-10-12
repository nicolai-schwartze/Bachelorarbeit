
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

void reniceTask(int nicenessInt, int PID) {
    if(nicenessInt < -20 || nicenessInt > 19) {
        printf("niceness value not recognised \n");
        return;
    }
    char nicenessChar[4];
    if(nicenessInt < -9) {
        nicenessChar[0] = '-';
        nicenessChar[1] = (char)((int)((-nicenessInt)/10)+48);
        nicenessChar[2] = (char)((int)((-nicenessInt)%10)+48);
        nicenessChar[3] = '\0';
    }
    else if (nicenessInt >= -9 && nicenessInt < 0) {
        nicenessChar[0] = '-';
        nicenessChar[1] = (char)((int)((-nicenessInt)%10)+48);
        nicenessChar[2] = '\0';
    }
    else if (nicenessInt >= 0 && nicenessInt < 10) {
        nicenessChar[0] = (char)(nicenessInt+48);
        nicenessChar[1] = '\0';
    }
    else if (nicenessInt >= 10) {
        nicenessChar[0] = (char)((int)((nicenessInt)/10)+48);
        nicenessChar[1] = (char)((int)((nicenessInt)%10)+48);
        nicenessChar[2] = '\0';
    }
    else {
        printf("niceness not recognised \n");
    }
    
    if(PID < 2) {
        printf("not a valid PID \n");
    }
    int length = snprintf(NULL, 0, "%d", PID) + 1;
    char PIDChar[length];
    snprintf(PIDChar, length, "%d", PID);
    
    char command[30];
    strcpy(command, "sudo renice ");
    strcat(command, nicenessChar);
    strcat(command, " -p ");
    strcat(command, PIDChar);
    
    system(command);
    
}

int main (void) {
    
    int integer;
    float floateger;
    double longfloateger;
    long long int longlonginteger;
    _Bool _Boolteger;
    short int shortinteger;
    char charteger;
    
    printf("sizeof(int) =...........%i \n", sizeof(integer));
    printf("sizeof(float) =         %i \n", sizeof(floateger));
    printf("sizeof(long float) =....%i \n", sizeof(longfloateger));
    printf("sizeof(long long int) = %i \n", sizeof(longlonginteger));
    printf("sizeof(_Bool) =.........%i \n", sizeof(_Boolteger));
    printf("sizeof(short int) =     %i \n", sizeof(shortinteger));
    printf("sizeof(char) =..........%i \n", sizeof(charteger));
    
    reniceTask(-10, 2309);
    
    
    return 0;
}