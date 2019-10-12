#include <stdio.h>

int main() {
    
    _Bool boolean = 1;
    int integer = -8949;
    boolean = (_Bool) integer;
    printf("boolean = %d \n", boolean);
    
    return 0;
}