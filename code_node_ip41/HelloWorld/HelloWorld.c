#include <stdio.h>
#include <unistd.h>

int main(void) {
    for (int i = 0; i < 10; i++) {
        printf("Hello World\n");
        sleep(1);
        
    }
    
    return 0;
}