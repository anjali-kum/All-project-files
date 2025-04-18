/************************User Defined Header files************************/
#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H
/********************System Defined Header Files*************************/
#include <Arduino.h>
/*********************Varialbe Declaration******************************/
extern const int stepPin;
extern const int dirPin;
extern const int enPin;
extern volatile uint8_t statement;
/*************************Function Declaration**************************/
void startMotor();
void stopMotor();
void idleMotor();
void readstatement(volatile uint8_t &statement, const char *receivedData);

#endif
