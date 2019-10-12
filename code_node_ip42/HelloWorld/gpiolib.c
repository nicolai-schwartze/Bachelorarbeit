#include "gpiolib.h"

int digitalWrite(int pin, _Bool val){
	//check for a correct pin number
	if((pin < 1) || (pin > 89)) {
		printf("[digitalWrite function]: Error: Not a pin\n");
		return -1;
	}
	char pinC[] = "00";
	//convert the pin into a string
    pinC[0] = (((int)(pin/10)) + '0');
	pinC[1] = ((pin - (10*((int)(pin/10)))) + '0');
	char filepath[35] = "/dev/null"; 
	//open file path with pin string
    strcpy(filepath, "/sys/class/gpio/gpio");
    strcat(filepath, pinC);
    strcat(filepath, "/value");
    
    //open file pin with the option write
	FILE *IO_value = NULL;
	IO_value = fopen (filepath, "w");
	//write the passed _Bool value to the file
	if(val) {
	    fwrite ("1", 1, sizeof("1"), IO_value);
	}
	if(!val) {
	    fwrite ("0", 1, sizeof("0"), IO_value);
	}
	//close the file 
    fclose (IO_value);
    return 0;
}


_Bool digitalRead(int pin) {
	//check for correct pinnumber
		if((pin < 1) || (pin > 89)) {
			printf("[digitalRead function]: Error: Not a pin\n");
			return NULL;
	}
	
	//Variable declaration
	_Bool valB = 0;
	char valC [2] = "0\0";
	
	//convert pin int to string
	char pinC[] = "00";
    pinC[0] = (((int)(pin/10)) + '0');
	pinC[1] = ((pin - (10*((int)(pin/10)))) + '0');
	
	//generate file path from pin number
	char valPath [35] = "/dev/null";
	strcpy(valPath, "/sys/class/gpio/gpio");
    strcat(valPath, pinC);
    strcat(valPath, "/value");
    
    //open file with option read
    FILE *IO_value = NULL;
    IO_value = fopen(valPath, "r");
    fread(&valC, 1, sizeof("1"), IO_value);
    //close file
    fclose(IO_value);
    //return value
    if(valC[0] == '1') {
    	valB = 1;
    }
    else if (valC[0] == '0') {
    	valB = 0;
    }
    else {
    	return NULL;
    }
	return valB;
}

//initialise GPIO
int openGPIO (int pin, char pinDirection) {
	//check for sensible input
	if ((pin > 89) || (pin < 1)) {
	    printf("[openGPIO function]: Error: Not a pin\n");
	    return -1;
	}
	if (!((pinDirection == 'o') || (pinDirection == 'i'))){
		printf("[opneGPIO function]: Error Not a valid option\n"); 
		return -1;
	}
	
	//convert int to string
	char pinC[] = "00";
    pinC[0] = (((int)(pin/10)) + '0');
	pinC[1] = ((pin - (10*((int)(pin/10)))) + '0');
	
	//write string to file export
	FILE *export_file = NULL;
	export_file = fopen ("/sys/class/gpio/export", "w");
   	fwrite (pinC, 1, sizeof(pinC), export_file);
   	fclose (export_file);
   	
   	//write output or input to file with pin number
   	char directionPath [35] = "/dev/null";
   	strcpy(directionPath, "/sys/class/gpio/gpio");
    strcat(directionPath, pinC);
    strcat(directionPath, "/direction");
    
    FILE *IO_direction = NULL;
    IO_direction = fopen(directionPath, "w");
    
    if(pinDirection == 'o'){
    	fwrite("out", 1, sizeof("out"), IO_direction);
    	fclose(IO_direction);
    }
    if(pinDirection == 'i') {
    	fwrite("in", 1, sizeof("in"), IO_direction);
    	fclose(IO_direction);
    }
	return 0;

}


int closeGPIO (int pin) {
	//check for sensible input
	if ((pin > 89) || (pin < 1)) {
	    printf("[closeGPIO function]: Error: Not a pin\n");
	    return -1;
	}
	//unexport file
	char pinC[] = "00";
    pinC[0] = (((int)(pin/10)) + '0');
	pinC[1] = ((pin - (10*((int)(pin/10)))) + '0');
	FILE *export_file;
    export_file = fopen ("/sys/class/gpio/unexport", "w");   
    fwrite (pinC, 1, sizeof(pinC), export_file);
    fclose (export_file);
    return 0;
}