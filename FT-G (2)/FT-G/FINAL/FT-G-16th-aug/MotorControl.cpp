#include "MotorControl.h"
#include "UARTHandler.h"
#include <Arduino.h>

// Pin definitions for motor control
const int stepPin = 5;  // Stepper motor step pin
const int dirPin = 2;   // Stepper motor direction pin
const int enPin = 8;    // Stepper motor enable pin

// State variables for motor operation
bool motorRunning = false;
unsigned long previousMillis = 0;  // Stores the last time the motor timing was updated
unsigned long runDuration = 0;     // Duration for which the motor should run

// Command to indicate motor has stopped (GotoStart)
uint8_t GotoStartt[] = { 0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00 };

// Flags to indicate if minutes and seconds have been set
bool minSet = false;
bool secSet = false;

// Function to start the motor
void startMotor() {
  static bool motorStarted = false;

  // Only print the start message once when the motor starts
  if (!motorStarted) {
    const char *startMsg = "Stepper motor starting...\n";
    sendDebugMessage(startMsg);
    motorStarted = true;
  }

  digitalWrite(dirPin, HIGH);      // Set the motor direction to clockwise
  digitalWrite(enPin, LOW);        // Enable the motor driver
  for (int i = 0; i < 400; i++) {  // Move the motor with a specific speed (400 steps)
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
}

// Function to stop the motor
void stopMotor() {
  static bool motorStopped = false;

  // Only print the stop message once when the motor stops
  if (!motorStopped) {
    const char *stopMsg = "Stepper motor stopping...\n";
    sendDebugMessage(stopMsg);
    motorStopped = true;
  }

  motorRunning = false;        // Update the motor running status
  digitalWrite(stepPin, LOW);  // Stop the motor by setting the step pin low
  sendDebugMessage("Motor stopped.\n");

  // Send a command to indicate that the motor has stopped
  sendDebugMessage((char *)(GotoStartt));
  sendDebugMessage((char *)(GotoStartt));


  // Send reset values to DWIN display
    sendResetValuesToDWIN();
    homePage_setSec = 0;
    homePage_setMin = 0;

    // Reset the flags for minSet and secSet
    minSet = false;
    secSet = false;

}

// Function to update motor timing and check if the motor should stop
void updateTiming() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - previousMillis;

  // Send a debug message with the elapsed time and run duration

  char debugMsg[50];
  snprintf(debugMsg, sizeof(debugMsg), "Elapsed Time: %lu ms, Run Duration: %lu ms\n", elapsedTime, runDuration);
  sendDebugMessage(debugMsg);

  // Check if the motor has run for the specified duration
  if (elapsedTime >= runDuration) {
    stopMotor();  // Stop the motor when the run duration is reached

    // Send the GotoStart command via UART to indicate the motor has stopped
    for (size_t i = 0; i < sizeof(GotoStartt); i++) {
      UART_send(GotoStartt[i]);
    }
    for (size_t i = 0; i < sizeof(GotoStartt); i++) {
      UART_send(GotoStartt[i]);
    }

    statements = 2;  // Update the motor state
   // minSet = false;  // Reset the minute flag
   // secSet = false;  // Reset the second flag


    // homePage_setSec = 0;
    // homePage_setMin = 0;

    sendResetValuesToDWIN();
    homePage_setSec = 0;
    homePage_setMin = 0;
    minSet = false;  // Reset the minute flag
    secSet = false;  // Reset the second flag
  }
}


// Function to convert milliseconds to seconds
unsigned long millisToSeconds(unsigned long millis) {
  return millis / 1000;
}

// Function to convert milliseconds to minutes
unsigned long millisToMinutes(unsigned long millis) {
  return millis / 60000;
}

// Function to set the motor's run duration based on minutes and seconds
void setMotorRunDuration(unsigned int minutes, unsigned int seconds) {
  runDuration = (minutes * 60000) + (seconds * 1000);  // Calculate the run duration in milliseconds

  // Send a debug message with the set run duration
  char debugMsg[50];
  snprintf(debugMsg, sizeof(debugMsg), "Run Duration Set: %lu ms\n", runDuration);
  sendDebugMessage(debugMsg);
}
