#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

// Motor control pin definitions
extern const int stepPin;
extern const int dirPin;
extern const int enPin;
extern bool motorRunning;
extern unsigned long previousMillis;
extern unsigned long runDuration;

// Flags for command completeness
extern bool minSet;
extern bool secSet;
extern const uint8_t GotoStart[];
extern const size_t GotoStartSize;

// Function prototypes
void startMotor();
void stopMotor();
void updateTiming();
void resetMotorState();
unsigned long millisToSeconds(unsigned long millis);
unsigned long millisToMinutes(unsigned long millis);
void setMotorRunDuration(unsigned int minutes, unsigned int seconds);


#endif  // MOTORCONTROL_H
