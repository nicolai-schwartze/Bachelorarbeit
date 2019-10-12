#ifndef __GPIOLIB_H_INCLUDED__
#define __GPIOLIB_H_INCLUDED__

#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int digitalWrite(int pin, _Bool val);
int openGPIO (int pin, char pinDirection);
int closeGPIO (int pin);
_Bool digitalRead(int pin);

#endif
