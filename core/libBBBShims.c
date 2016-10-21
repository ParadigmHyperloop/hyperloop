#include "libBBB.h"

// GPIO Prototypes
int initPin(int pinnum) { return 0; }
int setPinDirection(int pinnum, char *dir) { return 0; }
int setPinValue(int pinnum, int value) { return 0; }
int digitalWrite(int pinnum, int value) { return 0; }
int getPinValue(int pinnum) { return 0; }

// PWM Prototypes
int initPWM(int mgrnum, char *pin) { return 0; }
int setPWMPeriod(int helpnum, char *pin, int period) { return 0; }
int setPWMDuty(int helpnum, char *pin, int duty) { return 0; }
int setPWMOnOff(int helpnum, char *pin, int run) { return 0; }

// UART Prototypes
int initUART(int mgrnum, char *uartnum) { return 0; }
void closeUART(int fd) {}
int configUART(UART u, int property, char *value) { return 0; }
int txUART(int uart, unsigned char data) { return 0; }
unsigned char rxUART(int uart) { return '\0'; }
int strUART(int uart, char *buf) { return 0; }

// I2C Prototypes
int initI2C(int modnum, int addr) { return 0; }
void closeI2C(int device) { return; }
int writeByteI2C(int device, unsigned char *data) { return 0; }
int writeBufferI2C(int device, unsigned char *buf, int len) { return 0; }
int readByteI2C(int device, unsigned char *data) { return 0; }
int readBufferI2C(int device, int numbytes, unsigned char *buf) { return 0; }

// SPI Prototypes
int initSPI(int modnum) { return 0; }
void closeSPI(int device) { return; }
int writeByteSPI(int device, unsigned char *data) { return 0; }
int writeBufferSPI(int device, unsigned char *buf, int len) { return 0; }
int readByteSPI(int device, unsigned char *data) { return 0; }
int readBufferSPI(int device, int numbytes, unsigned char *buf) { return 0; }

// LCD 4-bit Prototypes
int initLCD() { return 0; }
int writeChar(unsigned char data) { return 0; }
int writeCMD(unsigned char cmd) { return 0; }
int writeString(char *str, int len) { return 0; }
int LCD_ClearScreen() { return 0; }
int LCD_Home() { return 0; }
int LCD_CR() { return 0; }
int LCD_Backspace() { return 0; }
int LCD_Move(int location) { return 0; }

// ADC Prototypes
int initADC(int mgrnum) { return 0; }
int readADC(int helpnum, char *ach) { return 0; }

// Time Prototypes
void pauseSec(int sec) { return; }
int pauseNanoSec(long nano) { return 0; }
