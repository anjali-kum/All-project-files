/******************Include Header Files*************************************/
#include "MotorControl.h"
/******************Variable Declaration and initialization******************/
const int stepPin = 5; 
const int dirPin = 2; 
const int enPin = 8;

volatile uint8_t statement = 0;
bool motorStarted = false;  // Track motor state
/**********************Function for starting Motor*************************/
void startMotor() {
  if (!motorStarted) {
    // Print statement for starting the motor
    const char *startMsg = "Stepper motor starting...\n";
    for (int i = 0; startMsg[i] != '\0'; i++) {
      while (!(UCSR0A & (1 << UDRE0)));
      UDR0 = startMsg[i];
    }
    motorStarted = true;
  }

  digitalWrite(dirPin, HIGH); // Set the direction to clockwise
  for (int x = 0; x < 800; x++) {
    digitalWrite(stepPin, HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin, LOW); 
    delayMicroseconds(500); 
  }
}
/********************Function for Stopping Motor*******************************/
void stopMotor() {
  if (motorStarted) {
    // Print statement for stopping the motor
    const char *stopMsg = "Stepper motor stopping...\n";
    for (int i = 0; stopMsg[i] != '\0'; i++) {
      while (!(UCSR0A & (1 << UDRE0)));
      UDR0 = stopMsg[i];
    }
    motorStarted = false;
  }

  digitalWrite(stepPin, LOW); // Stop the motor
}
/**********************Idle State of Motor*************************************/
void idleMotor() {
  // Do nothing (motor remains idle)
}
/**********************readstatements function*******************************/
void readstatement(volatile uint8_t &statement, const char *receivedData) {
  const char startStr[] = {0x01, 0x02, 0x03, 0x04, 0x00};
  const char stopStr[] = {0x04, 0x04, 0x04, 0x04, 0x00};

  if (strcmp(receivedData, startStr) == 0) {
    statement = 1;  // Start motor
  } else if (strcmp(receivedData, stopStr) == 0) {
    statement = 2;  // Stop motor
  } else {
    statement = 0;  // Idle state
  }
}

/***********************************Note*************************************/
/*
USART_RX_vect ISR definition, likely because HardwareSerial library in Arduino is already using it.
One solution is to use a different USART serial port if your microcontroller has more than one. 
However, if you're using an ATmega328p (such as on the Arduino Uno), which only has one USART,
you can avoid the conflict by not using the Serial object provided by the Arduino core and implementing the serial communication manually.
*/
