#include "gpiolib.h"

int main() {
	
	
	openGPIO(47, 'i');
	openGPIO(45, 'o');
	_Bool input = 0;
	for (int i = 0; i <1000; i++) {
	    digitalWrite(45, 1);
	    sleep(1);
	    printf("pin 45 = %d \n", digitalRead(47));
	}
	closeGPIO(47);
	closeGPIO(48);

}