/*************User Define Header Files****************************/
#ifndef UARTHANDLER_H
#define UARTHANDLER_H
/***********Header Files****************************************/
#include <Arduino.h>
/**************************User Define Macros*******************/
#define BAUD_RATE 9600
#define MYUBRR F_CPU/16/BAUD_RATE-1
/************************Global Variables************************/
extern char receivedData[20];
extern volatile int buffercnt;
extern char outputBuffer[50];
extern volatile bool flag;
/**************************Function Declaration******************/
void handleReceivedData();
void handleEcho();

#endif
