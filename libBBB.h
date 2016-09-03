/*
//Filename: libBBB.h
//Version : 0.1
//
//Project : Argonne National Lab - Forest
//Author  : Gavin Strunk
//Contact : gavin.strunk@gmail.com
//Date    : 28 June 2013
//
//Description - This is the main header file for
//		the libBBB library.
//
//Revision History
//	0.1:  Wrote basic framework with function
//		prototypes and definitions. \GS
*/

/*
Copyright (C) 2013 Gavin Strunk

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#ifndef _libBBB_H_
#define _libBBB_H_

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

//Type definitions
typedef struct {
      struct termios u;
}UART;

//Definitions
#define OUT	"out"
#define IN	"in"
#define ON	1
#define OFF	0

#define USR1	"usr1"
#define USR2	"usr2"
#define USR3	"usr3"
#define P8_13	"P8_13"
#define E	65
#define RS	27
#define D4	46
#define D5	47
#define D6	26
#define D7	44
#define AIN0	"/AIN0"
#define AIN1	"/AIN1"
#define AIN2	"/AIN2"
#define AIN3	"/AIN3"
#define AIN4	"/AIN4"
#define AIN5	"/AIN5"
#define AIN6	"/AIN6"
#define AIN7	"/AIN7"

//Device Tree Overlay
int addOverlay(char *dtb, char *overname);

//USR Prototypes
int setUsrLedValue(char* led, int value);

//GPIO Prototypes
int initPin(int pinnum);
int setPinDirection(int pinnum, char* dir);
int setPinValue(int pinnum, int value);
int getPinValue(int pinnum);

//PWM Prototypes
int initPWM(int mgrnum, char* pin);
int setPWMPeriod(int helpnum, char* pin, int period);
int setPWMDuty(int helpnum, char* pin, int duty);
int setPWMOnOff(int helpnum, char* pin, int run);

//UART Prototypes
int initUART(int mgrnum, char* uartnum);
void closeUART(int fd);
int configUART(UART u, int property, char* value);
int txUART(int uart, unsigned char data);
unsigned char rxUART(int uart);
int strUART(int uart, char* buf);

//I2C Prototypes
int initI2C(int modnum, int addr);
void closeI2C(int device);
int writeByteI2C(int device, unsigned char *data);
int writeBufferI2C(int device, unsigned char *buf, int len);
int readByteI2C(int device, unsigned char *data);
int readBufferI2C(int device, int numbytes, unsigned char *buf);

//SPI Prototypes
int initSPI(int modnum);
void closeSPI(int device);
int writeByteSPI(int device,unsigned char *data);
int writeBufferSPI(int device, unsigned char *buf, int len);
int readByteSPI(int device, unsigned char *data);
int readBufferSPI(int device, int numbytes, unsigned char *buf);

//LCD 4-bit Prototypes
int initLCD();
int writeChar(unsigned char data);
int writeCMD(unsigned char cmd);
int writeString(char* str, int len);
int LCD_ClearScreen();
int LCD_Home();
int LCD_CR();
int LCD_Backspace();
int LCD_Move(int location);

//ADC Prototypes
int initADC(int mgrnum);
int readADC(int helpnum, char* ach);

//Time Prototypes
void pauseSec(int sec);
int  pauseNanoSec(long nano);

#endif
